#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_COMMAND_NOT_FOUND 127
#define EXIT_BINARY_CORRUPTION 132 // binary corruption
#define EXIT_ABORTED 133 // dividng an int by zero
#define EXIT_FAILED_ASSERTION 134
#define EXIT_TOO_MUCH_MEMORY 137
#define SEGMENTATION_FAULT 139

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

struct Shell {
		const char* Cursor = "$";
		char* CommandBuf;
} Shell;

typedef struct process {
		struct process *next; //next in pipeline
		char **argv;
		pid_t pid; // process id
		bool completed;
		bool stopped;
		int status;
} process;

typedef struct job {
		struct job *next; //next job
		char *command;
		process *first_process;
		pid_t pgid;
		bool notified;
		struct termios tmodes;
		int stdin, stdout, stderr;
} job;

job *first_job = NULL;

job*
find_job (pid_t pgid)
{
		job* j;
		for(j=first_job; j; j = j->next) {
				if(j->pgid == pgid) return j;
		}
		return NULL;
}

bool
job_is_stopped(job* j)
{
		process* p;

		for(p=j->first_process; p; p=p->next) {
				if(!p->completed && !p->stopped) return false;
		}
		return true;
}

bool
job_is_completed(job* j)
{
		process* p;

		for(p=j->first_process; p; p=p->next) {
				if(!p->completed) return false;
		}
		return true;
}



void
init_shell()
{
		shell_terminal = STDIN_FILENO;
		shell_is_interactive = isatty (shell_terminal);

		if(shell_is_interactive)
		{
				//while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
						//kill (- shell_pgid, SIGTTIN);
				
				signal (SIGINT, SIG_IGN);
				signal (SIGQUIT, SIG_IGN);
				signal (SIGTSTP, SIG_IGN);
				signal (SIGTTIN, SIG_IGN);
				signal (SIGTTOU, SIG_IGN);
				signal (SIGCHLD, SIG_IGN);
		}
}

pid_t 
call(char* argv[])
{
		pid_t pid = fork();
		
		if(pid==-1)
		{
				exit(EXIT_FAILURE);
		}
		if(pid==0)
		{
				static char *newargv[] = {NULL, NULL};
				static char *newenviron[] = {NULL};

				newargv[0] = argv[1];
				newargv[1] = "Hello";

				execve(argv[1], newargv, newenviron);
				
				perror("Could not execve\n");
				exit(EXIT_FAILURE);
		}
		else
		{
				return pid;
		}
}

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

int
main(int argc, char* argv[])
{


		/*Shell.CommandBuf = (char*)malloc(sizeof(char) * 2);
		if(argc != 2)
		{
				fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
				exit(EXIT_FAILURE);
		}




		for(;;)
		{
				fprintf(stdout, "%s ", Shell.Cursor);
				sgets(argv[1], 128);

				pid_t pid = call(argv);

		}*/
		init_shell();

		return EXIT_SUCCESS;
}
