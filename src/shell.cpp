#include "../include/shell.h"
#include "../include/jobs.h"

static void signal_handler(int signum, siginfo_t *sip, void *context);

void
init_shell()
{
		// Not using open(2) now b/c the stdin seems to be enough
		// might need to in future
		Shell.terminal = STDIN_FILENO;
		Shell.is_interactive = isatty(Shell.terminal);
		Shell.device_name = ttyname(Shell.terminal);
		ioctl(0, TIOCGWINSZ, &Shell.winsize); // get terminal size
		
		yung_clog(INFO, "Shell is interactive: %d", Shell.is_interactive);
		yung_clog(INFO, "Device name: %s ", Shell.device_name);
		yung_clog(INFO, "Lines %d", Shell.winsize.ws_row);

		if(Shell.is_interactive) {
				
				// loop to foreground
				while(tcgetpgrp(Shell.terminal) != (Shell.pgid = getpgrp()))
						kill(- Shell.pgid, SIGTTIN);
				
				/* Ignore interactive and job-control signals.  */
				signal (SIGINT, SIG_IGN);
				signal (SIGTSTP, SIG_IGN);
				signal (SIGQUIT, SIG_IGN);
				signal (SIGTTIN, SIG_IGN);
				signal (SIGTTOU, SIG_IGN);

				struct sigaction sa;
		
				sa.sa_sigaction = signal_handler;
				sigemptyset(&sa.sa_mask);
				sa.sa_flags = SA_RESTART | SA_SIGINFO;

				sig_atomic_t sigsarr[2] = {SIGCHLD, SIGCONT};
				
				u8 i;
				for(i=0;i<2;i++)
						if(sigaction(sigsarr[i], &sa, (struct sigaction *)NULL) == -1)
								yung_clog(ERROR, 
										"Sigaction failure: couldn't init SIGCHLD link to signal_handler");


				// put shell in own process group
				Shell.pgid = getpid();
				if(setpgid(Shell.pgid, Shell.pgid) < 0) {
						yung_clog(FATAL, 
										"Couldn't put shell in it's own process group.");
						exit(EXIT_FAILURE);
				}
				
				// grab control of term
				tcsetpgrp(Shell.terminal, Shell.pgid);
				// save default term attribs for shell
				tcgetattr(Shell.terminal, &Shell.ORIGINAL_TMODES);

				memcpy(&Shell.tmodes, &Shell.ORIGINAL_TMODES, sizeof(Shell.ORIGINAL_TMODES));


				//new_t.c_lflag &= ~(ISIG);
				Shell.tmodes.c_lflag &= ~(ICANON | ECHO | IEXTEN);
				Shell.tmodes.c_cc[VTIME] = 0; // 
				Shell.tmodes.c_cc[VMIN] = 1; // min # of chars
				Shell.tmodes.c_cc[VINTR] = 1;  // ETX
				Shell.tmodes.c_cc[VERASE] = 1; // DEL or BS
				Shell.tmodes.c_cc[NOFLSH] = 0;
				Shell.tmodes.c_cc[VLNEXT] = 1; // Ctrl+V
				Shell.tmodes.c_cc[VEOF] = 1; // EOF
				//new_t.c_iflag &= (IUTF8);
				//new_t.c_cflag;
				//new_t.c_oflag &= (IUTF8);
				// OLCUC for all uppercase, shout!
				//cc_t c_cc[NCCS];
				//shell_tmodes.c_oflag = 0;

				if(cfsetispeed(&Shell.tmodes, B9600) < 0 
								|| cfsetospeed(&Shell.tmodes, B9600) < 0){ }
				
				if(tcsetattr(Shell.terminal, TCSANOW, &Shell.tmodes) < 0) { }
		}
}

/*	Return sigchld codes:= EXITED = 1, KILLED, DUMPED, TRAPPED, STOPPED, CONTINUED
 *	
 *
 */
static void 
signal_handler(int signum, siginfo_t *sip, void *context)
{
    switch (signum) {
        case SIGCHLD: //sent to parent when child stops
						yung_clog(DEBUG, 
										"SIGCHLD %d emmitted for process: %d; exited with code: %d; status: %d",
										sip->si_signo, sip->si_pid, sip->si_code, sip->si_status);
						do_job_notification();
						break;
				case SIGCONT:
						do_job_notification(); // might have to fix
						break;
    }
}

void
reset_terminal()
{
		printf("%cm", ESC); //reset color changes
		fflush(stdout);
		tcsetattr(Shell.terminal, TCSANOW, &Shell.ORIGINAL_TMODES);
}

void
hide_cursor() {printf("%c[?25l", ESC);}

#include <term.h>
void
getCursorPosition(int* rows, int* cols)
{
		char buf[32];
		u8 i = 0;

		write(STDOUT_FILENO, "\x1b[6n", 4);


    // Read the response
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    // Parse the response
    if (buf[0] == '\x1b' && buf[1] == '[') {
        sscanf(&buf[2], "%d;%d", rows, cols);
    }
}
