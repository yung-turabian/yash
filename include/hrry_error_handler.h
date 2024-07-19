#include "hrry_stdinc.h"

inline bool report(int line, const char * where, const char * message)
{
		fprintf(stderr, "[line %d ] Error%s : %s",
						line, where, message);
		return true;
}

inline void error(int line, const char * message)
{
		report
}
