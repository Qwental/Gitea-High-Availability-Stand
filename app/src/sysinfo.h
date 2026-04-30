#ifndef SYSINFO_H
#define SYSINFO_H

#define INFO_BUF_SIZE 4096
#include <stddef.h>
typedef struct {
    char hostname[256];
    char os_release[256];
    char kernel[256];
    long uptime_sec;
    long total_ram_mb;
    long free_ram_mb;
    int  cpu_count;
    char timestamp[32];
} SystemInfo;

int collect_sysinfo(SystemInfo *info);
int format_sysinfo_json(const SystemInfo *info, char *buf, size_t size);

#endif