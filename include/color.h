#define ANSI_RED 1
#define ANSI_GREEN 2
#define ANSI_YELLOW 3
#define ANSI_BLUE 4
#define ANSI_MAGENTA 5
#define ANSI_CYAN 6
#define ANSI_WHITE 7
#define ANSI_RESET "\x1b[0m"

#define cprintf(text, color) printf("\x1b[%dm%s%s", 30+color,text,ANSI_RESET)
#define cfbprintf(text, fg, bg) printf("\x1b[%dm\x1b[%dm%s%s", 30+fg, 40+bg,text,ANSI_RESET)

