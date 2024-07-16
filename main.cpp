// Components
#include "include/YaSH_stdinc.h"
#include "include/builtin.h"
#include "include/shell.h"
#include "include/jobs.h"
#include "include/YaSH_x11.h"

#include <thread>
#include <mutex>

std::mutex buffer_mutex;

#define MAX_TOKENS 6
#define MAX_STRING 80

#define quit "❌"
#define ls "👀"

/*
 *	Integrate the SHELL.CPP and JOBS.CPP
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

		p->next = NULL;
		p->argv = (char**)malloc(4096 * sizeof(char*));
		p->argv = argv;
		p->completed = false;
		p->stopped = false;
		
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

void
handle_input(char* buf, u8 *buf_len, bool *enter)
{

		unsigned char* emoji;
		// pretty shit, rework
		char c;
		do {
				//fprintf(stdout, "%c[2K\r", ASCII_ESC);
				fprintf(stdout, "%c8", ASCII_ESC);
				fprintf(stdout, "%s%c[0K", buf, ASCII_ESC);
				//printf("%d", buf_len);
				fflush(stdout);

				c = getchar();

				switch(c) {
						case 3: //^C
								//kill(id, 9);
								//Break up job processing to be able to identify and kill
								break;
						case 9: //TAB
								break;
						case 10: //LF
								*enter = true;
								break;
						case 12: //FF, CTRL-L
								fprintf(stdout, "%c[2J%c[1;1H🐚 %c7", 
												ASCII_ESC,ASCII_ESC,ASCII_ESC); 
								break;
						case 127: //DEL, very broken 
						case 8: //BS
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
										printf("[yash🐚] Not implemented yet!\n");
										break;
								}
								break;
						default:
								strncat(buf, &c, 1);
								(*buf_len)++;
								break;
				}
		} while(c != EOF && !(*enter));
}

int 
main(int argc, char* argv[])
{
		if (setenv("TERM", "xterm-256color", 1) != 0) {
        return 1;
    }

		//get();
		init_shell();

		BufferedLine* currentLine;
		DIR *current_dir;
		//struct dirent *dp;

		if((current_dir = opendir(".")) == NULL) {
				fprintf(stderr, "[🐚yash] couldn't open '.'");
				return EXIT_FAILURE;
		}
		
		
		char* buf;
		char* hist;

		u8 buf_len;
		char* path = (char*)"/usr/bin/";
		char full_cmd[50] = "";
		char* shell_argv[2];

		buf = (char*)malloc(sizeof(char) * 4096);
		hist = (char*)malloc(sizeof(char) * 4096);
		buf_len = 0;

		bool enter = false;


		// FIX THIS PLEAS!!!
		for(;;)
		{
				do_job_notification();
				fprintf(stdout, "🐚 %c7", ASCII_ESC);

				handle_input(buf, &buf_len, &enter);

				if(enter) {
						execute_command(buf, &buf_len);
						buf_len = 0;
						buf[0] = '\0';

						enter = false;
				} else {
						fprintf(stdout, "\n"); //no input
				}
				
				
				if(buf[0] != '\0') {
						currentLine = tok(buf, " ");
						link(currentLine->tokens, currentLine->argc);

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
						
						fprintf(stdout, "\n");

						// This is really bad!
						if(cmd != NULL) {
								int res = execute(shell_argv);
								if(res == -1) {

										//if(lookup(cmd, path)) {
												//strcat(full_cmd, path); 
												//strcat(full_cmd, cmd);
												//shell_argv[0] = full_cmd;


										call(shell_argv);

										strcat(hist, shell_argv[0]);
										//} 
								} 
								else if(res == EXIT_CALLED_FROM_CMD) {	break; }
								else {
										
								}

						}
						
						full_cmd[0] = '\0';
				} 
		}


		

		// Cleanup
		free(currentLine->tokens);
		free(buf);
		free(currentLine);
		closedir(current_dir);

		
		
		return(EXIT_SUCCESS);
}
