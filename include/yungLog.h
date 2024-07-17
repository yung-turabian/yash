#ifndef YUNGLOG_H
#define YUNGLOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define COLOR_RESET "\033[0m"

typedef enum AnsiiColor_t {
		INFO = 0,
		DEBUG,
		TRACE,
		WARN,
		ERROR,
		FATAL
} AnsiiColor_t;

inline FILE *LogFptr;

/** Logs a line of text */
inline void 
yungLog(AnsiiColor_t type, const char *fmt, ...) 
{
    va_list ap;
    time_t t;
    char datestr[51];

    // Determine if we just go to stdout
		FILE* file = (LogFptr == NULL) ? stdout : LogFptr;
		if(file == NULL) {
				fprintf(stdout, 
								"[yungLog] Can't set to stdout or file pointer\n");
				exit(EXIT_FAILURE);
		}
		
		char fmt_type[30];
		switch(type) {
				case INFO:
						if(file == stdout)
								strcpy(fmt_type, "\033[34m[INFO]"); // Blue
						else
								strcpy(fmt_type, "[INFO]");
						break;
				case DEBUG:
						if(file == stdout)
								strcpy(fmt_type, "\033[35m[DEBUG]"); // Cyan
						else
								strcpy(fmt_type, "[DEBUG]");
						break;
				case TRACE:
						if(file == stdout)
								strcpy(fmt_type, "\033[32m[TRACE]"); // Greem
						else
								strcpy(fmt_type, "[TRACE]");
						break;
				case WARN:
						if(file == stdout)
								strcpy(fmt_type, "\033[33m[WARN]"); // Yellow
						else
								strcpy(fmt_type, "[WARN]");
						break;
				case ERROR:
						if(file == stdout)
								strcpy(fmt_type, "\033[31m[ERROR]"); // Red
						else
								strcpy(fmt_type, "[ERROR]");
						break;
				case FATAL:
						if(file == stdout)
								strcpy(fmt_type, "\033[41m\033[37m[FATAL]"); // White on Red
						else
								strcpy(fmt_type, "[FATAL]");
						break;
				default:
						strcpy(fmt_type, "[YUNG_LOG]"); // White
		}

    // Get current time and format it
    t = time(NULL);
    tzset();
    strftime(datestr, sizeof(datestr) - 1, "%Y/%m/%d %T", localtime(&t));
		if(file == stdout) 
				fprintf(file, "\x1b[38;5;241m%s %s:%s ", 
								datestr, fmt_type, COLOR_RESET);
		else
				fprintf(file, "%s %s: ", datestr, fmt_type);
		
		// Handle variable arguments
    va_start(ap, fmt);
    vfprintf(file, fmt, ap);
    va_end(ap);

    // Move to the next line
    fprintf(file, "\n");
		fflush(file);
}

inline void
yungLog_create_fp(const char* AppName)
{

    time_t t;
    char datestr[51];

		t = time(NULL);
		tzset();
    strftime(datestr, sizeof(datestr) - 1, "%Y-%m-%d.log", localtime(&t));
		int len = sizeof(AppName) + sizeof(datestr);
		char buf[sizeof(AppName) + sizeof(datestr)];
		snprintf(buf, len, "%s_%s", AppName, datestr);
		
		LogFptr = fopen(buf, "w"); // needs to clean up!
}


#define log(...)     			yungLog(NULL, __VA_ARGS__)
#define clog(type, ...)   yungLog(type, __VA_ARGS__)


/*
// Logging levels
inline void log_trace(const char *fmt, ...) {
    printf(COLOR_TRACE "[TRACE] ");
    va_list args;
    va_start(args, fmt);
    yung_log(NULL, fmt, args);
    va_end(args);
    printf(COLOR_RESET);
}

inline void log_debug(const char *fmt, ...) {
    printf(COLOR_DEBUG "[DEBUG] ");
    va_list args;
    va_start(args, fmt);
    yung_log(NULL, fmt, args);
    va_end(args);
    printf(COLOR_RESET);
}

inline void log_info(const char *fmt, ...) {
    printf(COLOR_INFO "[INFO] ");
    va_list args;
    va_start(args, fmt);
    yung_log(NULL, fmt, args);
    va_end(args);
    printf(COLOR_RESET);
}

inline void log_warn(const char *fmt, ...) {
    printf(COLOR_WARN "[WARN] ");
    va_list args;
    va_start(args, fmt);
    yung_log(NULL, fmt, args);
    va_end(args);
    printf(COLOR_RESET);
}

inline void log_error(const char *fmt, ...) {
    printf(COLOR_ERROR "[ERROR] ");
    va_list args;
    va_start(args, fmt);
    yung_log(NULL, fmt, args);
    va_end(args);
    printf(COLOR_RESET);
}

inline void log_fatal(const char *fmt, ...) {
    printf(COLOR_FATAL "[FATAL] ");
    va_list args;
    va_start(args, fmt);
    yung_log(NULL, fmt, args);
    va_end(args);
    printf(COLOR_RESET);
}*/

#ifdef __cplusplus
}
#endif

#endif // YUNGLOG_H
