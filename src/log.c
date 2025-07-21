#include "log.h"
#include <stdarg.h>
#include <string.h>

static const char * const log_labels[] = {
	"[FATAL] ", "[ERROR] ", "[WARNING] ", "[INFO] ", "[DEBUG] "
};

FILE *log_output = NULL;

void _vlog(LogLevel level, const char *fmt, ...)
{
	if(log_output == NULL)
		log_output = stdout;

	fwrite(log_labels[level], 1, strlen(log_labels[level]), log_output);

	va_list args;
	va_start(args, fmt);
	
	vfprintf(log_output, fmt, args);
	
	va_end(args);

	fputc('\n', log_output);
}

