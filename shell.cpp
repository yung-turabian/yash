#include "include/shell.h"
#include "include/jobs.h"

void
init_shell()
{
		// Not using open(2) now b/c the stdin seems to be enough
		// might need to in future
		Shell.terminal = STDIN_FILENO;
		Shell.is_interactive = isatty(Shell.terminal);
		Shell.device_name = ttyname(Shell.terminal);
		ioctl(0, TIOCGWINSZ, &Shell.winsize); // get terminal size
		
		// Debug print
		fprintf(stdout, "[yash🐚] Shell is interactive: %d \n"
										"	 Device name: %s \n"
										"  Lines %d\n", Shell.is_interactive, 
										Shell.device_name, Shell.winsize.ws_row);

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
				signal (SIGCHLD, SIG_IGN);
				// VERASE ^H or ^?
				// VSTOP AND VSTART, ^S and ^Q

				// put shell in own process group
				Shell.pgid = getpid();
				if(setpgid(Shell.pgid, Shell.pgid) < 0) {
						fprintf(stderr, 
										"[yash🐚] Couldn't put shell in it's own process group, ask Henry\n");
						exit(EXIT_FAILURE);
				}
				
				// grab control of term
				tcsetpgrp(Shell.terminal, Shell.pgid);
				// save default term attribs for shell
				tcgetattr(Shell.terminal, &Shell.tmodes);

				struct termios new_tmodes;
				memcpy(&new_tmodes, &Shell.tmodes, sizeof(new_tmodes));


				//new_t.c_lflag &= ~(ISIG);
				new_tmodes.c_lflag &= ~(ICANON | ECHO);
				new_tmodes.c_cc[VTIME] = 0;
				new_tmodes.c_cc[VMIN] = 1;
				new_tmodes.c_cc[VERASE] = 1;
				//new_t.c_iflag &= (IUTF8);
				//new_t.c_cflag;
				//new_t.c_oflag &= (IUTF8);
				// OLCUC for all uppercase, shout!
				//cc_t c_cc[NCCS];
				//shell_tmodes.c_oflag = 0;

				if(cfsetispeed(&new_tmodes, B9600) < 0 || cfsetospeed(&new_tmodes, B9600) < 0){ }
				
				if(tcsetattr(Shell.terminal, TCSANOW, &new_tmodes) < 0) { }

				//printf("\e[?25l"); // hides cursor
				atexit(reset_terminal);
		}



}

void
reset_terminal()
{
		printf("\em"); //reset color changes
		fflush(stdout);
		tcsetattr(Shell.terminal, TCSANOW, &Shell.tmodes);
}
