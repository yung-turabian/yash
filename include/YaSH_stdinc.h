/**
 *	\file YaSH_stdinc.h
 *
 *	General header that includes C/Unix/C++ language
 */

#ifndef YASH_STDINC_H
#define YASH_STDINC_H

// Unix system
#include <sys/wait.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
// C libs
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
// Extra libs
#include <ncurses.h>

// Exit codes, for child processes and program
#ifndef CHILD_EXIT_CODE
		#define CHILD_EXIT_CODE 42
#endif

#define EXPECTED_CHILD_EXIT_CODE 42

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_COMMAND_NOT_FOUND 127
#define EXIT_BINARY_CORRUPTION 132 // binary corruption
#define EXIT_ABORTED 133 // dividng an int by zero
#define EXIT_FAILED_ASSERTION 134
#define EXIT_TOO_MUCH_MEMORY 137
#define SEGMENTATION_FAULT 139

#define EXIT_CALLED_FROM_CMD 69

// Typedefs
typedef uint8_t u8;

#endif
