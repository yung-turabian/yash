/**
 *	\file hrry.cpp
 *
 *	The entrypoint for the shell, houses main loop.
 *
 */

// Components
#include "../include/hrry_stdinc.h"
#include "../include/builtin.h"
#include "../include/shell.h"
#include "../include/jobs.h"

#ifdef _WIN32
		
#elif __APPLE__
		
#elif __linux__
		#include "../include/linux/hrry_x11.h"
#endif

#define MAX_TOKENS 10
#define MAX_STRING 80

static bool shouldExit = false;

/*
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
execute(int argc, char **argv)
{
		int i;

		if(argv[0] == NULL) {
				return 1; // empty command
		}

		for(i = 0; i < num_builtins(); i++) {
				if(strcmp(argv[0], builtin_directory[i]) == 0) {
						return (*builtin_func[i])(argc, argv);
				}
		}

		return -1;
}

job*
call(int argc, char **argv)
{
		job* j = NULL;
		process *p = NULL;

		j = (job*)malloc(sizeof(job));
    if (j == NULL) {
        yung_clog(ERROR,"New job failed to allocate.");
        return NULL;
    }

		p = (process*)malloc(sizeof(process));
    if (p == NULL) {
        yung_clog(ERROR,"New process failed to allocate.");
				free(j);
        return NULL;
    }

		p->argv = (char**)malloc((argc + 1) * (MAX_STRING + 1));
    if (p->argv == NULL) {
        yung_clog(ERROR, "Failed to allocate memory for a new process's argv.");
        free(p);
        free(j);
        return NULL;
    }

		for(int i = 0; i < argc; i++) {
				p->argv[i] = strdup(argv[i]);
				if (p->argv[i] == NULL) {
            yung_clog(ERROR, "Failed to duplicate argv string.");
            for (int j = 0; j < i; j++) {
                free(p->argv[j]);
            }
            free(p->argv);
            free(p);
            free(j);
            return NULL;
        }
		}
		p->argv[argc] = NULL; // null terminate so execvp works
		
		p->next = NULL;
		p->completed = false;
		p->stopped = false;
		
		j->command = strdup(argv[0]);
    if (j->command == NULL) {
        yung_clog(ERROR, "Failed to duplicate command string.");
        for (int i = 0; i < argc; i++) {
            free(p->argv[i]);
        }
        free(p->argv);
        free(p);
        free(j);
        return NULL;
    }

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

// for use of running a .hrry script
/*void
runFile(const char * path) 
{
		// Read bytes
		// run()
}*/

typedef struct BufferedLine {
		char** tokens;
		u8 argc;

		char* buf;
		u8 buf_len;
} BufferedLine;

void execute_command(BufferedLine* currentLine);

#include "../include/io.h"


void
free_tokens(char** tokens, u8 count)
{
		u8 i;
		for(i=0;i<count;i++)
				free(tokens[i]);

		free(tokens);
}

// Use of the hrry lang in interactive shell mode
void
runPrompt()
{
		BufferedLine* currentLine = (BufferedLine*)malloc(sizeof(BufferedLine));
		if(currentLine == NULL) {
				yung_clog(ERROR, "Failure to allocate for buffered line struct.");
				return;
		}

		currentLine->buf = (char*)malloc(sizeof(char) * 4096);
		if(currentLine == NULL) {
				yung_clog(ERROR, "Failure to allocate line reader buffer.");
				free(currentLine);
				return;
		}
		currentLine->buf[0] = '\0';
		//hist = (char**)malloc((sizeof(char) * 4096) * 10); write this to a file instead
		currentLine->buf_len = 0;
		currentLine->tokens = NULL;
		currentLine->argc = 0;

		// FIX THIS PLEASE!!!
		for(;;)
		{
				if (shouldExit)
						break;

				currentLine->buf_len = 0;
				currentLine->buf[0] = '\0';
				fprintf(stdout, "🐚 %c7", ESC);
				handle_input(currentLine->buf, &currentLine->buf_len);

				if(currentLine->buf[0] != '\0') {
						execute_command(currentLine);
				} else {
						fprintf(stdout, "\n"); //no input
				}
		}

		// on exit
		free(currentLine->buf);

		if(currentLine->tokens != NULL)
				free_tokens(currentLine->tokens, currentLine->argc);
		
		free(currentLine);
}

void
run(const char * source)
{
		// list of tokens

}


void
tok(char* str, const char* del, char*** tokens, u8* count)
{
		yung_clog(INFO, "Beginning a tokenizer task.");
		char* token;
		int i;

		*tokens = NULL;
		*count = 0;

		token = strtok(str, del);
		if(token == NULL) {
				yung_clog(INFO, 
						"Closed tokenizer task as there were no tokens in the buffer.");
				return;
		}

		*tokens = (char**)malloc(MAX_TOKENS * (MAX_STRING + 1));
		if(tokens == NULL) {
				yung_clog(ERROR, "Tokenizer failed to allocate for provided token buffer.");
				return;
		} 

		for(i=0; i < MAX_TOKENS && token != NULL; i++) {
				(*tokens)[i] = (char*)malloc(MAX_STRING + 1);
				if((*tokens)[i] == NULL) {
						yung_clog(ERROR, "Tokenizer failed to allocate for a token.");
						
						free_tokens(*tokens, *count);
						yung_clog(INFO, 
										"Freed token buffers and all tokens (including shitty ones).");
						return;
				}
					
				strcpy((*tokens)[i], token);
				token = strtok(NULL, del);
				(*count)++;
		}
		
		*tokens = (char**)realloc(*tokens, (*count) * (MAX_STRING + 1));
		if(*tokens == NULL) {
				yung_clog(ERROR,
						"Tokenizer failed to reallocate token buffer.");
				return;
		}

		yung_clog(INFO, 
						"Successfully finished tokenizer task and found %d tokens.", *count);
}

#include "../include/linker.h"

#define MAX_NUMBER_ARGS 4096
#define MAX_ARG_SIZE 100


void
execute_command(BufferedLine* currentLine)
{
		if(currentLine->buf_len != 0) {
				tok(currentLine->buf, " ", &currentLine->tokens, &currentLine->argc);

				if(currentLine->tokens == NULL || currentLine->argc == 0)
						return;

				fflush(stdout);
				char *argv[MAX_NUMBER_ARGS]; 
				int argc = 0;		

				link(currentLine->tokens, currentLine->argc);

				for(argc=0;argc < currentLine->argc;argc++) 
						argv[argc] = (char*)currentLine->tokens[argc];

				fprintf(stdout, "\n");

				// This is really bad!
				if(argv[0] != NULL) {
						int res = execute(argc, argv);
						if(res == -1) {

								// default alias, move this 
								if(strcmp(argv[0], "ls") == 0) {
										argv[argc++] = (char*)"--color=auto";
										argv[argc++] = (char*)"-F";
								}

								call(argc, argv);

								//strcat(hist, shell_argv[0]);
						} 
						else if(res == EXIT_CALLED_FROM_CMD)
								shouldExit = true;
				} 

		}

		free_tokens(currentLine->tokens, currentLine->argc);
		currentLine->tokens = NULL;

}

/*
#include <pthread.h>

void* job_notification_thread(void *arg) {
    while (1) {
        do_job_notification();
        sleep(1);  // Sleep for a second to avoid busy-waiting
    }
    return NULL;
}*/

// Whether to use emoji scripting or regular syntactic styling
bool runStd = false;

int 
main(int argc, char* argv[])
{
		if(argc > 1) {
				if(strncmp(argv[1], "-v", 2) == 0) {
						fprintf(stdout, "[hrry] version 0.1.0\n");
						return EXIT_SUCCESS;
				} else if(strncmp(argv[1], "--help", 6) == 0) {
						system("less +g -R man/hrry_help.txt");
						return EXIT_SUCCESS;
				} else if(strncmp(argv[1], "--std", 6) == 0) {
						runStd = true;
				}
		}



		if (setenv("TERM", "xterm-256color", 1) != 0) {
        return 1;
    }


		yungLog_fopen("hrry");
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


		runPrompt();




		

		// Cleanup
		//closedir(current_dir);
		reset_terminal();
		do_job_notification();
		yungLog_fclose();
		
		
		return(EXIT_SUCCESS);
}
