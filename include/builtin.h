/**
 *	\file YaSH_stdinc.h
 *
 *	General header that includes C/Unix/C++ language
 */

#ifndef YASH_BUILTIN_H
#define YASH_BUILTIN_H

#include "YaSH_stdinc.h"


int cd(char* argsv[]);

int help(char* argsv[]);

int exit(char* argv[]);

static char* builtin_directory[] = {
		(char*)"cd",
		(char*)"help",
		(char*)"exit"
};

static int(*builtin_func[]) (char**) = {
		&cd,
		&help,
		&exit
};

inline int num_builtins() 
		{ return sizeof(builtin_directory) / sizeof(char*); }

#endif
