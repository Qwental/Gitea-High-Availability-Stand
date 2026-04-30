#include "sysinfo.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

int collect_sysinfo(SystemInfo *info)
{
    if (!info) return -1;
    memset(info, 0, sizeof(*info));

    gethostname(info->hostname, sizeof(info->hostname) - 1);

    struct utsname uts;
    if (uname(&uts) == 0) {
        snprintf(info->os_release, sizeof(info->os_release),
                 "%s %s", uts.sysname, uts.release);
        snprintf(info->kernel, sizeof(info->kernel), "%s", uts.version);
    }

    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        info->uptime_sec   = si.uptime;
        info->total_ram_mb = (long)(si.totalram * si.mem_unit) / (1024 * 1024);
        info->free_ram_mb  = (long)(si.freeram  * si.mem_unit) / (1024 * 1024);
    }

    info->cpu_count = (int)sysconf(_SC_NPROCESSORS_ONLN);

    time_t now = time(NULL);
    strftime(info->timestamp, sizeof(info->timestamp),
             "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    return 0;
}

int format_sysinfo_json(const SystemInfo *info, char *buf, size_t size)
{
    return snprintf(buf, size,
        "{"
        "\"hostname\":\"%s\","
        "\"os_release\":\"%s\","
        "\"kernel\":\"%s\","
        "\"uptime_sec\":%ld,"
        "\"total_ram_mb\":%ld,"
        "\"free_ram_mb\":%ld,"
        "\"cpu_count\":%d,"
        "\"timestamp\":\"%s\""
        "}\n",
        info->hostname, info->os_release, info->kernel,
        info->uptime_sec, info->total_ram_mb, info->free_ram_mb,
        info->cpu_count, info->timestamp);
}