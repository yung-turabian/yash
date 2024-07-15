// Components
#include "include/YaSH_stdinc.h"
#include "include/builtin.h"
#include "util.cpp"

#define MAX_TOKENS 6
#define MAX_STRING 80

#define quit "❌"
#define ls "👀"

/*
 *	Just setup working wit NCurses, handling termios stuff seems a little
 *	like a waste of time.
 *
 *
 *	Redirection. Probably just detect if its a file and then open an fstream and write to
 *	that file :)
 *	
 *	use execveat(2) if passed a path as an additiona argument
 *	https://man7.org/linux/man-pages/man2/execveat.2.html
 *
 *	
 *	Figure out piping
 *	https://man7.org/linux/man-pages/man2/pipe.2.html
 *
*		Clean up kids and job management
 *
 */

void 
sgets(char* str, int n)
{
		char* str_read = fgets(str, n, stdin);
		if(!str_read) return;

		int i=0;
		while(str[i] != '\n' && str[i] != '\0')
		{
				i++;
		}

		if(str[i] == '\n') str[i] = '\0';
}

pid_t call(char* argv[]);

int
execute(char* argv[])
{
		int i;

		if(argv[0] == NULL) {
				return 1; // empty command
		}

		for(i = 0; i < num_builtins(); i++) {
				if(strcmp(argv[0], builtin_directory[i]) == 0) {
						return (*builtin_func[i])(argv);
				}
		}

		//return call(argv);
		return -1;
}

size_t iteration = 0;

pid_t
call(char* argv[])
{
		pid_t pid;
		FILE* fptr;

		fptr = fopen("out.debug", "w");
		if(fptr == nullptr) {
				fprintf(stdout, "[io] failed to open debug file\n");
				exit(EXIT_FAILURE);
		}
		fprintf(fptr, "---%zu----------\n", iteration++);
		
		if(signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
				perror("signal");
				exit(EXIT_FAILURE);
		}

		pid = fork();
		switch(pid) {
		case -1:
				perror("fork");
				return(EXIT_FAILURE);
		
		case 0: 
				fprintf(fptr, "Child: my internal pid is %d. \e[0m\n", pid);
				fprintf(fptr, "Child: Exiting with exit code %d. \e[0m\n", CHILD_EXIT_CODE);
				
				static char *newargv[] = {NULL, NULL, NULL, NULL};
				static char *newenviron[] = {NULL};

				newargv[0] = argv[0];
				newargv[1] = argv[1];
				
				if(execve(argv[0], newargv, newenviron) == -1) {
						perror("execve");
				}
				exit(EXIT_FAILURE); // only reached if perror is called
		
		default: 
				int status;

				fprintf(fptr, "Parent: my child is %d.\n", pid);
				fprintf(fptr, "Parent: Waiting for my child [%d].\n", pid);

				waitpid(pid, &status, 0);

				fprintf(fptr, "Parent: my child exited with status %d.\n", status);
				if(WIFEXITED(status))
				{
						fprintf(fptr, "Parent: my child's exit code is %d.\n", WIFEXITED(status));

						if(WEXITSTATUS(status) == EXPECTED_CHILD_EXIT_CODE) {
								fprintf(fptr, "Parent: that's the code I expected.\n");
						} 
						else {
								fprintf(fptr, "Parent: that exit code is unexpected...\n");
						}

				}
				

		}
		
		fprintf(fptr, "\n\n");
		fclose(fptr);
		return pid;
}

typedef struct BufferedLine {
		char** tokens;
		int argc;
} BufferedLine;

BufferedLine* 
tok(char* str, const char* del)
{
		char* token;
		token = strtok(str, del);
		
		BufferedLine* currentLine;
		currentLine = (BufferedLine*)malloc(sizeof(BufferedLine));
		currentLine->tokens = (char**)malloc(sizeof(char*) * MAX_TOKENS); //max
		currentLine->argc = 1;

		for(int i=0; i < MAX_TOKENS; i++) {
				currentLine->tokens[i] = (char*)malloc(MAX_STRING + 1);
				if(currentLine->tokens[i] == NULL) return NULL;
					
				strcpy(currentLine->tokens[i], token);
				
				token = strtok(NULL, del);

				if(token == NULL) break;
				currentLine->argc++;
		}

		return currentLine;
}

bool
lookup(const char* arg, const char* path)
{
		DIR* dirp;
		struct dirent *dp;

		if((dirp = opendir(path)) == NULL) {
				fprintf(stderr, "[🐚yash] couldn't open '%s'", path);
				exit(EXIT_FAILURE);
		}

		do {
				errno = 0;
				if((dp = readdir(dirp)) != NULL) {
						if(strcmp(dp->d_name, arg) != 0) continue;

						closedir(dirp);
						return true;
				}

		} while(dp != NULL);

		if(errno != 0) {
				perror("error reading directory");
		}
		else {
				fprintf(stdout, "[🐚yash] Unknown command: %s\n", arg);
		}
		
		closedir(dirp);
		return false;
}

#include <fcntl.h>

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_is_interactive, shell_terminal;
char* device_name;
int fd;

void
reset_terminal()
{
		printf("\em"); //reset color changes
		fflush(stdout);
		tcsetattr(shell_terminal, TCSANOW, &shell_tmodes);
}

void
init_shell()
{
		// Not using open(2) now b/c the stdin seems to be enough
		// might need to in future
		shell_terminal = STDIN_FILENO;

		shell_is_interactive = isatty(shell_terminal);
		device_name = ttyname(shell_terminal);

		fprintf(stdout, "[yash🐚] Shell is interactive: %d \n"
										"	 Device name: %s \n", shell_is_interactive, device_name);

		if(shell_is_interactive) {
				
				// loop to foreground
				while(tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
						kill(- shell_pgid, SIGTTIN);
				
				/* Ignore interactive and job-control signals.  */
				//signal (SIGINT, SIG_IGN); // ^C
				signal (SIGQUIT, SIG_IGN);
				signal (SIGTSTP, SIG_IGN); // ^Z
				signal (SIGTTIN, SIG_IGN);
				signal (SIGTTOU, SIG_IGN);
				signal (SIGCHLD, SIG_IGN);
				// VERASE ^H or ^?
				// VSTOP AND VSTART, ^S and ^Q

				shell_pgid = getpid();
				if(setpgid(shell_pgid, shell_pgid) < 0) {
						fprintf(stderr, 
										"[yash🐚] Couldn't put shell in it's own process group, ask Henry\n");
						exit(EXIT_FAILURE);
				}
				
				// grab control of term
				tcsetpgrp(shell_terminal, shell_pgid);
				// save default term attribs for shell
				tcgetattr(shell_terminal, &shell_tmodes);

				struct termios new_t = shell_tmodes;


				//new_t.c_lflag &= ~(ISIG);
				//new_t.c_lflag &= ~(ICANON);
				//new_t.c_iflag &= (IUTF8);
				//new_t.c_cflag;
				//new_t.c_oflag &= (IUTF8);
				// OLCUC for all uppercase, shout!
				//cc_t c_cc[NCCS];
				//shell_tmodes.c_oflag = 0; 
				
				if(tcsetattr(shell_terminal, TCSANOW, &new_t) < 0) { }

				//printf("\e[?25l"); // hides cursor
				atexit(reset_terminal);
		}



}

int
read_key(char* buf, int k)
{
		if(buf[k] == '\033' && buf[k + 1] == '[') {
				switch(buf[k + 2]) {
						case 'A': return 1; // up
						case 'B': return 2; // down
						case 'C': return 3; // right
						case 'D': return 4; // left
				}
		}
		return -1; //something went wrong
}

int 
main(int argc, char* argv[])
{

		init_shell();
	
		BufferedLine* currentLine;
		DIR *current_dir;
		//struct dirent *dp;

		if((current_dir = opendir(".")) == NULL) {
				fprintf(stderr, "[🐚yash] couldn't open '.'");
				return EXIT_FAILURE;
		}
		
		
		char* buf;
		char* path = (char*)"/usr/bin/";
		char full_cmd[50] = "";
		char* shell_argv[2];

		buf = (char*)malloc(sizeof(char) * 4096);

		for(;;)
		{
				fprintf(stdout, "🐚 ");
				sgets(buf, 4096);


				currentLine = tok(buf, " ");

				if(strcmp(currentLine->tokens[0], "🗣️") == 0) { // For echo 🗣️, 
																					 // for echo with WavY eFfeCTs 😱
																					 // For pipe 👉,
						currentLine->tokens[0] = (char*)"echo";
				} else if(strcmp(currentLine->tokens[0], "😺") == 0) {
								currentLine->tokens[0] = (char*)"cat";
				}
				char* cmd = currentLine->tokens[0];
				
				char* arg;
				if(currentLine->argc > 1) {
						arg = currentLine->tokens[1];
				} else {
						arg = NULL;
				}
				shell_argv[1] = arg;

				// local command
				shell_argv[0] = cmd;
				
				// This is really bad!
				if(cmd != NULL) {
						int res = execute(shell_argv);
						if(res == -1) {

								if(lookup(cmd, path)) {
										strcat(full_cmd, path); 
										strcat(full_cmd, cmd);
										shell_argv[0] = full_cmd;


										call(shell_argv);
								} 
						} else if(res == EXIT_CALLED_FROM_CMD) {	break; }
						else {
								continue;
						}

				}
				
				full_cmd[0] = '\0';
		}


		

		// Cleanup
		free(currentLine->tokens);
		free(buf);
		free(currentLine);
		closedir(current_dir);

		
		
		return(EXIT_SUCCESS);
}
