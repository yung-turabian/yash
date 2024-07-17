// Components
#include "include/YaSH_stdinc.h"
#include "include/builtin.h"
#include "include/shell.h"
#include "include/jobs.h"
#include "include/YaSH_x11.h"

#define MAX_TOKENS 6
#define MAX_STRING 80

#define quit "❌"
#define ls "👀"

/*
 *	Add LOGGING with PLOG
 *
 *	Redirection. Probably just detect if its a file and then open an fstream and write to
 *	that file :)
 *	https://stackoverflow.com/questions/35569673/implementing-input-output-redirection-in-a-linux-shell-using-c
*	
*	https://www.gnu.org/software/bash/manual/html_node/Redirections.html#Redirecting-Output
 *	https://man7.org/linux/man-pages/man2/dup.2.html
 *
 *
 *	
 *	Figure out piping
 *	https://man7.org/linux/man-pages/man2/pipe.2.html
 *
*		Clean up kids and job management
 *
 */


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

		return -1;
}

job*
call(char *argv[])
{
		job* j = NULL;
		process *p;

		j = (job*)malloc(sizeof(job));
		p = (process*)malloc(sizeof(process));

    if (j == NULL || p == NULL) {
        perror("malloc");
        return NULL;
    }

		size_t arg_count = 0;
		while(argv[arg_count]) arg_count++;

		p->argv = (char**)malloc((arg_count + 1) * sizeof(char*));
		for(size_t i = 0; i < arg_count; i++)
				p->argv[i] = strdup(argv[i]);
		p->argv[arg_count] = NULL; // null terminate so execvp works
		
		p->next = NULL;
		p->completed = false;
		p->stopped = false;
		
		j->command = argv[0];
		j->pgid = 0;
		j->first_process = p;
		j->stdin = STDIN_FILENO;
		j->stdout = STDOUT_FILENO;
		j->stderr = STDERR_FILENO;
		
		
		// update linked list head
		j->next = first_job;
		first_job = j;
		
		launch_job(j, true);
		return j;
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


#include <term.h>


const char* YASH_ECHO = (const char*)"💬";

#define ASCII_ESC 27

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

#include "linker.cpp"

const char* hist[40];
int hist_len;
int curr_hist_sel = 0;

bool editing_line = false;

void
handle_input(char* buf, u8 *buf_len)
{
		bool breakout = false;
		unsigned char* emoji;
		// pretty shit, rework
		char c;
		do {
				//fprintf(stdout, "%c[2K\r", ASCII_ESC);
				fprintf(stdout, "%c8", ASCII_ESC);
				fprintf(stdout, "%s%c[0K", buf, ASCII_ESC);
				fflush(stdout);

				c = getchar();

				switch(c) {
						case 3: //^C
								if(strlen(buf) == 0) {
								}
								break;
						case 9: //TAB
								break;
						case 10: //LF
								breakout = true;
								editing_line = false;
								if(strlen(buf) != 0) {
										hist[hist_len++] = strdup(buf);
										hist[hist_len] = "";
										curr_hist_sel++;
								}
								break;
						case 12: //FF, CTRL-L
								fprintf(stdout, "%c[2J%c[1;1H🐚 %c7", 
												ASCII_ESC,ASCII_ESC,ASCII_ESC); 
								break;
						case 127: //DEL
						case 8: //BS
								editing_line = true;
								if(*buf_len > 0) buf[--(*buf_len)] = '\0';
								break;
						case 22: //CTRL-V, pretty sloppy
								emoji = getX11Clipboard();
								strcat(buf, (const char*) emoji);
								fprintf(stdout, "%s", emoji);
								fflush(stdout);
								*buf_len += strlen((const char*)emoji);
								break;
						/*case 'e':
								//debug, kinda janky
								char* argv[2];
								argv[0] = (char*)"efck-chat-keyboard/efck-chat-keyboard";
								call(argv);
								buf[0] = '\0';
								buf_len++;
								break;*/
						case ASCII_ESC:
								getchar(); //consume '['
								switch(getchar()) { 
										case 'A': //Up arrow
										if(!editing_line && curr_hist_sel > 0) {
												strcpy(buf, hist[--curr_hist_sel]);
												*buf_len = strlen(hist[curr_hist_sel]);
												editing_line = false;
										}

										break;

										case 'B':
										if(!editing_line && curr_hist_sel >= 0 && curr_hist_sel < hist_len)
												strcpy(buf, hist[++curr_hist_sel]);
												*buf_len = strlen(hist[curr_hist_sel]);
												editing_line = false;
										break;
								}
								break;
						default:
								editing_line = true;
								strncat(buf, &c, 1);
								(*buf_len)++;
								break;
				}
		} while(c != EOF && !(breakout));
}

void
execute_command(char* buf)
{
		BufferedLine* currentLine;
		if(buf[0] != '\0') {
				currentLine = tok(buf, " ");

				char* shell_argv[3]; // Needs to be null-terminated
				
				link(currentLine->tokens, currentLine->argc);

				char* cmd = currentLine->tokens[0];
				
				char* arg;
				if(currentLine->argc > 1) {
						arg = currentLine->tokens[1];
				} else {
						arg = NULL;
				}
				shell_argv[1] = arg;
				shell_argv[2] = NULL;
				

				// local command
				shell_argv[0] = cmd;
				
				fprintf(stdout, "\n");

				// This is really bad!
				if(cmd != NULL) {
						int res = execute(shell_argv);
						if(res == -1) {

								call(shell_argv);

								//strcat(hist, shell_argv[0]);
						} 
						else if(res == EXIT_CALLED_FROM_CMD) {	exit(EXIT_SUCCESS); }
						else {
								
						}
				} 

				free(currentLine->tokens);
				free(currentLine);
		}

}

#include <pthread.h>

void* job_notification_thread(void *arg) {
    while (1) {
        do_job_notification();
        sleep(1);  // Sleep for a second to avoid busy-waiting
    }
    return NULL;
}

int 
main(int argc, char* argv[])
{
		if(argc > 1 && strcmp(argv[1], "-v") == 0) {
				fprintf(stdout, "[YaSH🐚] version 0.1.0\n");
				return EXIT_SUCCESS;
		}


		if (setenv("TERM", "xterm-256color", 1) != 0) {
        return 1;
    }

		yungLog_create_fp("YaSH");

		init_shell();
		/*
    pthread_t notif_thread;
    if (pthread_create(&notif_thread, NULL, job_notification_thread, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }*/

		//DIR *current_dir;
		//struct dirent *dp;

		/*if((current_dir = opendir(".")) == NULL) {
				fprintf(stderr, "[🐚yash] couldn't open '.'");
				return EXIT_FAILURE;
		}*/
		
		
		char* buf;

		u8 buf_len;

		buf = (char*)malloc(sizeof(char) * 4096);
		buf[0] = '\0';
		//hist = (char**)malloc((sizeof(char) * 4096) * 10);
		buf_len = 0;
		hist_len = 0;


		// FIX THIS PLEAS!!!
		for(;;)
		{

				fprintf(stdout, "🐚 %c7", ASCII_ESC);

				handle_input(buf, &buf_len);

				if(buf[0] != '\0') {

						execute_command(buf);

						buf_len = 0;
						buf[0] = '\0';

				} else {
						fprintf(stdout, "\n"); //no input
				}

		}


		

		// Cleanup
		//closedir(current_dir);

		
		
		return(EXIT_SUCCESS);
}
