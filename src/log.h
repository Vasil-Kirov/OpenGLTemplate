#pragma once
#include <stdio.h>

typedef enum {
	LOG_FATAL,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
} LogLevel;


#define VINFO(fmt, ...) _vlog(LOG_INFO, fmt, __VA_ARGS__)
#define VWARN(fmt, ...) _vlog(LOG_WARN, fmt, __VA_ARGS__)
#define VERRO(fmt, ...) _vlog(LOG_ERROR, fmt, __VA_ARGS__)
#define VFATL(fmt, ...) _vlog(LOG_FATAL, fmt, __VA_ARGS__)

void _vlog(LogLevel level, const char *fmt, ...);
