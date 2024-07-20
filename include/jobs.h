#ifndef YASH_JOBS_H
#define YASH_JOBS_H
#include "hrry_stdinc.h"


inline job *first_job = NULL;

// associated with data struct, job
job* find_job(pid_t pgid);

// IFF true if all processes in a job are done
bool job_is_stopped(job *j);

bool job_is_completed(job* j);


void launch_process(process *p, pid_t pgid,
						   int infile, int outfile, int errfile,
							 bool foreground);

void launch_job(job *j, bool foreground);

// if cont is true, restore saved tmodes and send proces group a SIGCONT to wake it up
// before block
void put_job_in_foreground(job *j, bool cont);

// if cont arg is true, send process group a SIGCONT to wake it up
void put_job_in_background(job* j, bool cont);

// mark stopped job as running again
void mark_job_as_running(job *j);

void continue_job(job *j, bool foreground);


// Store status of process pid returned by waitpid
int mark_process_status(pid_t pid, int status);

// Check process that have status info available, won't block process
void update_status();

// Checks for porcesses that have status info, blocks until all processes in given job
// have reported
void wait_for_job(job *j);

void format_job_info(job *j, const char* status);

// Notify user about stopped/terminated jobs
void do_job_notification();

job* parse();
void free_job();

#endif
