
#include "include/jobs.h"



// Command to list all active jobs
// Command to send a SIGKILL to a job

// find active job with given pgid
job*
find_job(pid_t pgid)
{
		job *j;

		for(j=first_job; j; j = j->next) {
				if(j->pgid == pgid)
						return j;
		}
		return NULL;
}

// IFF true if all processes in a job are done
bool
job_is_stopped(job *j)
{
		process *p;

		for(p=j->first_process; p; p = p->next) {
				if(!p->completed && !p->stopped)
						return false;
		}
		return true;
}

bool
job_is_completed(job* j)
{
		process *p;

		for(p=j->first_process; p; p = p->next) {
				if(!p->completed)
						return false;
		}
		return true;
}


void
launch_process(process *p, pid_t pgid,
						   int infile, int outfile, int errfile,
							 bool foreground)
{
		pid_t pid;
		
		if(Shell.is_interactive) 
		{
				// Put process into The process group and give group the terminal, if appropriate
				//Has to be both by shell and in the indvid child processes b/c of potential
				// race conditions.
				pid = getpid();
				if(pgid == 0) pgid = pid;

				if(setpgid(pid, pgid) != 0)
						perror("setpgid() error");
				else {
						if(foreground) {

								printf("deubg\n");
								if(tcsetpgrp(Shell.terminal, pgid) != 0) {

										perror("tcsetpgrp() error");
								}
								else 
								{
								
										// Set the handling for job control signals back to the default.
										signal (SIGINT, SIG_DFL);
										signal (SIGQUIT, SIG_DFL);
										signal (SIGTSTP, SIG_DFL);
										signal (SIGTTIN, SIG_DFL);
										signal (SIGTTOU, SIG_DFL);
										signal (SIGCHLD, SIG_DFL);
										
										// config std i/o channels of new process.
										if(infile != STDIN_FILENO) {
												dup2(infile, STDIN_FILENO);
												close(infile);
										}
										if(outfile != STDOUT_FILENO) {
												dup2(outfile, STDOUT_FILENO);
												close(outfile);
										}
										if(errfile != STDERR_FILENO) {
												dup2(errfile, STDERR_FILENO);
												close(errfile);
										}

										// exec new process
										execvp(p->argv[0], p->argv); // this will take int account PATH var
										perror("execvp");
										exit(EXIT_FAILURE);
							}
						}
				}
		}
}

/*
pid_t
call(char* argv[])
{
		pid_t pid;
		
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
*/

void
launch_job(job *j, bool foreground)
{
		process *p;
		pid_t pid;
		int mypipe[2], infile, outfile;

		infile = j->stdin;
		for(p = j->first_process; p; p = p->next) 
		{
				// setup pipes, if necessary
				if(p->next) {
						if(pipe (mypipe) < 0) {
								perror("pipe");
								exit(EXIT_FAILURE);
						}
						outfile = mypipe[1];
				}
				else
						outfile = j->stdout;
				//check for builtin commands

				// Fork child processes.
				pid = fork();

				if(pid < 0) {//error
								perror("fork");
								exit(EXIT_FAILURE);
				}
				else if(pid == 0) {//child
						launch_process(p, j->pgid, infile,
												   outfile, j->stderr, foreground);
				}
				else {//parent
						p->pid = pid;
						if(Shell.is_interactive) {
								if(!j->pgid)
										j->pgid = pid;
								setpgid(pid, j->pgid);
						}

				}

				//cleanup pipes
				if(infile != j->stdin)
						close(infile);
				if(outfile != j->stdout)
						close(outfile);
				infile = mypipe[0];
		}

		format_job_info(j, "launched");

		if(!Shell.is_interactive)
				wait_for_job(j);
		else if(foreground)
				put_job_in_foreground(j,false);
		else
				put_job_in_background(j,false);
}

// if cont is true, restore saved tmodes and send proces group a SIGCONT to wake it up
// before block
void
put_job_in_foreground(job *j, bool cont)
{
		// puts job in foreground
		tcsetpgrp(Shell.terminal, j->pgid);
		
		// send job a continue signal if necessary
		if(cont) {
				tcsetattr(Shell.terminal, TCSADRAIN, &j->tmodes);
				if(kill(-j->pgid, SIGCONT) < 0)
						perror("kill (SIGCONT)");
		}
		

		// wait for report
		wait_for_job(j);
		
		// put shell back in foreground
		tcsetpgrp(Shell.terminal, Shell.pgid);
		tcgetattr(Shell.terminal, &j->tmodes);
		tcsetattr(Shell.terminal, TCSADRAIN, &Shell.tmodes);
}

// if cont arg is true, send process group a SIGCONT to wake it up
void
put_job_in_background(job* j, bool cont)
{
		if(cont)
				if(kill(-j->pgid, SIGCONT) < 0)
						perror("kill (SIGCONT)");
}

// mark stopped job as running again
void
mark_job_as_running(job *j)
{
		process *p;

		for(p=j->first_process; p; p = p->next)
				p->stopped = false;
		j->notified = 0;
}

void
continue_job(job *j, bool foreground)
{
		mark_job_as_running(j);
		if(foreground)
				put_job_in_foreground(j, true);
		else
				put_job_in_background(j, true);
}


// Store status of process pid returned by waitpid
int
mark_process_status(pid_t pid, int status)
{
		job *j;
		process *p;

		if(pid > 0)
		{
				// update record for process
				for(j=first_job; j; j=j->next) {
						for(p=j->first_process; p; p=p->next) 
						{
								if(p->pid == pid) {
										p->status = status;
										if(WIFSTOPPED(status))
												p->stopped = true;
										else {
												p->completed = true;
												if(WIFSIGNALED(status)) 
														fprintf(stderr, "%d: Terminated by signal %d. \n",
																		(int) pid, WTERMSIG(p->status));
										}
										return 0; //All is well!
								}
						fprintf(stderr, "No child process %d.\n", pid);
						return -1;
						}
				}
		} 
		else if(pid == 0 || errno == ECHILD) 
				// no processes ready to report
				return -1;
		else {
				// weird things going on here
				perror("waitpid");
				return -1;
		}
		return -1;
}

// Check process that have status info available, won't block process
void
update_status()
{
		int status;
		pid_t pid;

		do  //															stop/term     checks background jobs
				pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
		while(!mark_process_status(pid, status));
}

// Checks for processes that have status info, blocks until all processes in given job
// have reported
void
wait_for_job(job *j)
{
		int status;
		pid_t pid;

		do
				pid = waitpid(WAIT_ANY, &status, WUNTRACED);
		while(!mark_process_status(pid, status)
						&& !job_is_stopped(j)
						&& !job_is_completed(j));
}

void
format_job_info(job *j, const char* status)
{
		fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

void
free_job(job* j)
{
		process *p;
		process *previous_p;

		p = j->first_process;
		int i;
		while(p)
		{
				previous_p = p;
				p = p->next;

				i = 0;
				while(previous_p->argv[i])
						free(previous_p->argv[i++]);

				free(previous_p->argv);
				free(previous_p);
		}
		free(j);
}

// Notify user about stopped/terminated jobs
void
do_job_notification()
{
		job *j, *jlast, *jnext;

		update_status();

		jlast = NULL;
		for(j=first_job; j; j=jnext)
		{
				jnext = j->next;

				// If all process have completed, tell user and delete from list of active
				if(job_is_completed(j)) {
						format_job_info(j, "completed");
						if(jlast)
								jlast->next = jnext;
						else
								first_job = jnext;
						free_job(j);
				}

				// Notify about stopped jobs, marking them so don't repeat
				else if(job_is_stopped(j) && !j->notified) {
						format_job_info(j, "stopped");
						j->notified = 1;
						jlast = j;
				}
				
				// these jobs are still running
				else
						jlast = j;
		}
}

#define FD_DEFAULT 0
#define FD_STDIN 1
#define FD_STDOUT 2
#define FD_STDERR 3

int recollect(job *j, process* p, int* fdType, char* tempBuff, char* start, int length, int whitespace, int* cmd_index);
/*
job* parse()
{
		int failure = 0;
		job *j = NULL;
		process *p;

		j = (job*)malloc(sizeof(job));
		p = (process*)malloc(sizeof(process));

		p->next = NULL;
		p->argv = (char**)malloc(4096 * sizeof(char*));

		j->pgid = 0;
		j->first_process = p;
		j->stdin = STDIN_FILENO;
		j->stdout = STDOUT_FILENO;
		j->stderr = STDERR_FILENO;

		int fdType = FD_DEFAULT, length = 0, cmd_index = 0, whitespace = 0;
		char* c;
		char* start = buffer;
		char* tempBuff = (char*)malloc(4096 * sizeof(char));

		for(c = buffer; *c; ++c)
		{
				switch(*c) {
						case '|':
								failure = recollect(j, p, 
												&fdType, tempBuff, start, length, whitespace, &cmd_index);

								p->next = (process*)malloc(sizeof(process));
								p = p->next;
								p->next = NULL;
								p->argv = (char**)malloc(4096 * sizeof(char*));

								cmd_index = 0;
								whitespace = 1;
								fdType = FD_DEFAULT;
								start = c + 1;
								length = 0;
								break;
						case '>':
								failure = recollect(j, p, 
												&fdType, tempBuff, start, length, whitespace, &cmd_index);

								whitespace = 1;
								fdType = FD_STDOUT;
								start = c + 1;
								length = 0;
								break;
						case '<':
								failure = recollect(j, p, 
												&fdType, tempBuff, start, length, whitespace, &cmd_index);

								whitespace = 1;
								fdType = FD_STDIN;
								start = c + 1;
								length = 0;
								break;
						case ' ':
								failure = recollect(j, p, 
												&fdType, tempBuff, start, length, whitespace, &cmd_index);
								
								whitespace = 1;
								start = c + 1;
								length = 0;
								break;
						case '&':
								if(fdType != FD_DEFAULT)
										failure = 1;
								else
										put_job_in_background(j, true);

								whitespace = 1;
								start = c + 1;
								length = 0;
								break;

						default:
								whitespace = 0;
								length++;
								break;

				}
		}

		failure = recollect(j, p, &fdType, tempBuff, start, length, whitespace, &cmd_index);

		free(tempBuff);

		if(failure) {
				free_job(j);
				j = NULL;
		}

		return j;
}*/

// NOT MINE
int recollect(job *j, process* p, int* fdType, char* tempBuff, char* start, int length, int whitespace, int* cmd_index) {
	int failure = 0;

	switch(*fdType) {
		case FD_STDOUT:
			if(!whitespace) {
				strncpy(tempBuff, start, length);
				tempBuff[length] = '\0';
				j->stdout = open(tempBuff, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				if(j->stdout < 0) {
					printf("Error: Cannot redirect output.\n");
					failure = 1;
				}
				*fdType = FD_DEFAULT;
			}
			break;
		case FD_STDIN:
			if(!whitespace) {
				strncpy(tempBuff, start, length);
				tempBuff[length] = '\0';
				j->stdin = open(tempBuff, O_RDONLY);
				if(j->stdin < 0) {
					printf("Error: Cannot redirect input.\n");
					failure = 1;
				}
				*fdType = FD_DEFAULT;
			}
			break;
		default:
			if(!whitespace) {
				p->argv[*cmd_index] = (char*) malloc((length + 1) * sizeof(char));
				strncpy(p->argv[*cmd_index], start, length);
				p->argv[*cmd_index][length] = '\0';
				(*cmd_index)++;
				p->argv[*cmd_index] = NULL;
				*fdType = FD_DEFAULT;
			}
			break;
	}

	return failure;
}
