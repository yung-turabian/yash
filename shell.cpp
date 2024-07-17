#include "include/shell.h"
#include "include/jobs.h"

void signal_handler(int sig);

void
init_shell()
{
		// Not using open(2) now b/c the stdin seems to be enough
		// might need to in future
		Shell.terminal = STDIN_FILENO;
		Shell.is_interactive = isatty(Shell.terminal);
		Shell.device_name = ttyname(Shell.terminal);
		ioctl(0, TIOCGWINSZ, &Shell.winsize); // get terminal size
		
		struct sigaction sa;
		
		clog(INFO, "Shell is interactive: %d", Shell.is_interactive);
		clog(INFO, "Device name: %s ", Shell.device_name);
		clog(INFO, "Lines %d\n", Shell.winsize.ws_row);

		if(Shell.is_interactive) {
				
				// loop to foreground
				while(tcgetpgrp(Shell.terminal) != (Shell.pgid = getpgrp()))
						kill(- Shell.pgid, SIGTTIN);
				
				/* Ignore interactive and job-control signals.  */
				signal (SIGINT, SIG_IGN); // ^C
				signal (SIGQUIT, SIG_IGN);
				signal (SIGTSTP, SIG_IGN); // ^Z
				signal (SIGTTIN, SIG_IGN);
				signal (SIGTTOU, SIG_IGN);

				sa.sa_handler = signal_handler;
				sigemptyset(&sa.sa_mask);
				sa.sa_flags = SA_RESTART;

				sigaction(SIGCHLD, &sa, NULL);
				// VERASE ^H or ^?
				// VSTOP AND VSTART, ^S and ^Q

				// put shell in own process group
				Shell.pgid = getpid();
				if(setpgid(Shell.pgid, Shell.pgid) < 0) {
						fprintf(stderr, 
										"[yash🐚] Couldn't put shell in it's own process group, ask Henry\n");
						exit(EXIT_FAILURE);
				}
				//
				// grab control of term
				tcsetpgrp(Shell.terminal, Shell.pgid);
				// save default term attribs for shell
				tcgetattr(Shell.terminal, &Shell.ORIGINAL_TMODES);

				memcpy(&Shell.tmodes, &Shell.ORIGINAL_TMODES, sizeof(Shell.ORIGINAL_TMODES));


				//new_t.c_lflag &= ~(ISIG);
				Shell.tmodes.c_lflag &= ~(ICANON | ECHO);
				Shell.tmodes.c_cc[VTIME] = 0;
				Shell.tmodes.c_cc[VMIN] = 1;
				Shell.tmodes.c_cc[VERASE] = 1;
				//new_t.c_iflag &= (IUTF8);
				//new_t.c_cflag;
				//new_t.c_oflag &= (IUTF8);
				// OLCUC for all uppercase, shout!
				//cc_t c_cc[NCCS];
				//shell_tmodes.c_oflag = 0;

				if(cfsetispeed(&Shell.tmodes, B9600) < 0 
								|| cfsetospeed(&Shell.tmodes, B9600) < 0){ }
				
				if(tcsetattr(Shell.terminal, TCSANOW, &Shell.tmodes) < 0) { }

				//printf("\e[?25l"); // hides cursor
				atexit(reset_terminal);
		}



}

void
reset_terminal()
{
		printf("\em"); //reset color changes
		fflush(stdout);
		tcsetattr(Shell.terminal, TCSANOW, &Shell.ORIGINAL_TMODES);
}


void 
signal_handler(int sig) {
    switch (sig) {
        case SIGCHLD: //sent to parent when child stops
						do_job_notification();
            break;
        case SIGINT:
        case SIGTSTP:

            do_job_notification();
            break;
				case SIGCONT:

            do_job_notification();
						break;
    }
}
