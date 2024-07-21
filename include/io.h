#ifndef HRRY_IO_H
#define HRRY_IO_H

#include "hrry_stdinc.h"
#ifdef _WIN32
		
#elif __APPLE__
		
#elif __linux__
		#include "../include/linux/hrry_x11.h"
#endif

void handle_input(char* buf, u8 *buf_len);

#endif
