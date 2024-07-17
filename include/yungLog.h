#ifndef YUNGLOG_H
#define YUNGLOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
		COLOR_RESET = 0,
		INFO,
		DEBUG,
		TRACE,
		WARN,
		ERROR,
		FATAL
} AnsiiColor_t;

#define COLOR_RESET "\033[0m"

/** Logs a line of text */
inline void yung_log(FILE *file, AnsiiColor_t type, const char *fmt, ...) {
    va_list ap;
    time_t t;
    char datestr[51];

    // Determine if we just go to stdout
    file = (file == NULL) ? stdout : file;
		
		char color[30];
		switch(type) {
				case INFO:
						strcpy(color, "\033[34m[INFO]"); // Blue
						break;
				case DEBUG:
						strcpy(color, "\033[35m[DEBUG]"); // Blue
						break;
				case TRACE:
						strcpy(color, "\033[32m[TRACE]"); // Green
						break;
				case WARN:
						strcpy(color, "\033[33m[WARN]"); // Yellow
						break;
				case ERROR:
						strcpy(color, "\033[31m[ERROR]"); // Red
						break;
				case FATAL:
						strcpy(color, "\033[41m\033[37m[FATAL]"); // White on Red
						break;
				default:
						strcpy(color, "\033[37m[YUNG_LOG]"); // White
		}

    // Get current time and format it
    t = time(NULL);
    tzset();
    strftime(datestr, sizeof(datestr) - 1, "%b %d %T %Y", localtime(&t));
    fprintf(file, "\x1b[38;5;241m%s %s:%s ", 
						datestr, color, COLOR_RESET);
		
		// Handle variable arguments
    va_start(ap, fmt);
    vfprintf(file, fmt, ap);
    va_end(ap);

    // Move to the next line
    fprintf(file, "\n");
}

#define logf(f, ...) yung_log(f, NULL, __VA_ARGS__)
#define log(...)     yung_log(NULL, NULL, __VA_ARGS__)
#define clogf(f, type, ...) yung_log(f, type, __VA_ARGS__)
#define clog(type, ...)     yung_log(NULL, type, __VA_ARGS__)


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
