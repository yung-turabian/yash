#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

void test_fprintf(FILE *stream, const char* format, va_list args)
{
		char* test_msg = (char*)"[xtraTest] ";
		strcat(test_msg, format);
		fprintf(stream, test_msg, args);
}
    printf("%sred\n", KRED);
void test_equal_string()
{
		const char *str1 = "abc";
		const char *str2 = "abc";
		int result = strcmp(str1, str2);

		if(result != 0) {
				test_fprintf(stdout, "FAILED: test_equal_strings()\n", 10);
		}
}

void test_run_all(void)
{
		test_equal_string();
}
