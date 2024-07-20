/**
 *	\file hrry_stdinc.h
 *
 *	General header that includes C/Unix/C++ language
 */

#ifndef YASH_BUILTIN_H
#define YASH_BUILTIN_H

#include "hrry_stdinc.h"


int cd(int argc, char* argsv[]);

int help(int argc, char* argsv[]);

int exit(int argc, char* argv[]);

//int echo(int argc, char* argv[]);

static char* builtin_directory[] = {
		(char*)"cd",
		(char*)"help",
		(char*)"exit",
		//(char*)"echo",
};

static int(*builtin_func[]) (int, char**) = {
		&cd,
		&help,
		&exit,
		//&echo,
};

inline int num_builtins() 
		{ return sizeof(builtin_directory) / sizeof(char*); }

#endif
