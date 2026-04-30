#include "server.h"
#include "sysinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Логирование: Пишем в stdout построчно в формате JSON-lines
 * Такой формат подхватывают Filebeat, Fluent Bit, Vector без конфига парсинга
 */
static void log_json(const char *level, const char *msg)
{
    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    /* JSON-line: одна строка = одно событие */
    printf("{\"ts\":\"%s\",\"level\":\"%s\",\"msg\":\"%s\"}\n",
           ts, level, msg);
    fflush(stdout);
}

#define LOG_INFO(m)  log_json("INFO",  m)
#define LOG_ERROR(m) log_json("ERROR", m)

/* Сигналы  */

static volatile int g_running = 1;

static void on_signal(int sig)
{
    (void)sig;
    g_running = 0;
}

/*  HTTP  */

static void send_response(int fd, int code, const char *status,
                          const char *body)
{
    char hdr[256];
    int  hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        code, status, strlen(body));

    (void)write(fd, hdr, (size_t)hlen);
    (void) write(fd, body, strlen(body));
}

static void handle_client(int fd, const char *ip)
{
    char req[REQUEST_BUF_SIZE];
    char logbuf[512];
    char jsonbuf[INFO_BUF_SIZE];

    ssize_t n = read(fd, req, sizeof(req) - 1);
    if (n <= 0) { close(fd); return; }
    req[n] = '\0';

    char method[16] = {0}, path[256] = {0};
    sscanf(req, "%15s %255s", method, path);

    snprintf(logbuf, sizeof(logbuf), "%s %s from %s", method, path, ip);
    LOG_INFO(logbuf);

    if (strcmp(path, "/health") == 0 || strcmp(path, "/") == 0) {
        send_response(fd, 200, "OK", "{\"status\":\"ok\"}\n");

    } else if (strcmp(path, "/info") == 0) {
        SystemInfo si;
        if (collect_sysinfo(&si) != 0) {
            send_response(fd, 500, "Internal Server Error",
                          "{\"error\":\"sysinfo failed\"}\n");
        } else {
            format_sysinfo_json(&si, jsonbuf, sizeof(jsonbuf));
            send_response(fd, 200, "OK", jsonbuf);
        }

    } else {
        send_response(fd, 404, "Not Found", "{\"error\":\"not found\"}\n");
    }

    close(fd);
}


int server_run(uint16_t port)
{
    char logbuf[128];

    signal(SIGTERM, on_signal);
    signal(SIGINT,  on_signal);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { LOG_ERROR("socket() failed"); return -1; }

    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(port),
    };

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        snprintf(logbuf, sizeof(logbuf), "bind() port %u: %s",
                 port, strerror(errno));
        LOG_ERROR(logbuf);
        close(sfd);
        return -1;
    }

    listen(sfd, BACKLOG);

    snprintf(logbuf, sizeof(logbuf), "listening on 0.0.0.0:%u", port);
    LOG_INFO(logbuf);

    while (g_running) {
        struct sockaddr_in caddr;
        socklen_t clen = sizeof(caddr);

        int cfd = accept(sfd, (struct sockaddr *)&caddr, &clen);
        if (cfd < 0) break;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
        handle_client(cfd, ip);
    }

    LOG_INFO("shutdown");
    close(sfd);
    return 0;
}