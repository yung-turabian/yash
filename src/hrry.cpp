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

#define MAX_TOKENS 6
#define MAX_STRING 80

#define quit "❌"
#define ls "👀"

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

void
runFile(const char * path) 
{
		// Read bytes
		// run()
}

void execute_command(char* buf);

#include "../include/io.h"

const char* YASH_ECHO = (const char*)"💬";


void
runPrompt()
{

		char* buf;

		u8 buf_len;

		buf = (char*)malloc(sizeof(char) * 4096);
		buf[0] = '\0';
		//hist = (char**)malloc((sizeof(char) * 4096) * 10);
		buf_len = 0;

		// FIX THIS PLEAS!!!
		for(;;)
		{
				buf_len = 0;
				buf[0] = '\0';
				fprintf(stdout, "🐚 %c7", ESC);
				handle_input(buf, &buf_len);

				if(buf[0] != '\0') {

						execute_command(buf);
				} else {
						fprintf(stdout, "\n"); //no input
				}
		}
}

void
run(const char * source)
{
		// list of tokens

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

#include "../include/linker.h"

#define MAX_NUMBER_ARGS 4096
#define MAX_ARG_SIZE 100

void
execute_command(char* buf)
{
		BufferedLine* currentLine;
		if(buf[0] != '\0') {
				currentLine = tok(buf, " ");

				char *shell_argv[MAX_NUMBER_ARGS]; 
				// Needs to be null-terminated

				int argc;		

				link(currentLine->tokens, currentLine->argc);

				shell_argv[0] = currentLine->tokens[0];

				for(argc=1;argc < currentLine->argc;argc++)
						shell_argv[argc] = currentLine->tokens[argc];


				shell_argv[argc] = NULL;

				fprintf(stdout, "\n");

				// This is really bad!
				if(shell_argv[0] != NULL) {
						int res = execute(argc, shell_argv);
						if(res == -1) {

								call(argc, shell_argv);

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
		if(argc > 1) {
				if(strncmp(argv[1], "-v", 2) == 0) {
						fprintf(stdout, "[hrry] version 0.1.0\n");
						return EXIT_SUCCESS;
				} else if(strncmp(argv[1], "--help", 6) == 0) {
						system("less +g -R man/hrry_help.txt");
						return EXIT_SUCCESS;
				}
		}


		if (setenv("TERM", "xterm-256color", 1) != 0) {
        return 1;
    }

		yungLog_create_fp("hrry");

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

		
		
		return(EXIT_SUCCESS);
}
