#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <unistd.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_COMMAND_NOT_FOUND 127
#define EXIT_BINARY_CORRUPTION 132 // binary corruption
#define EXIT_ABORTED 133 // dividng an int by zero
#define EXIT_FAILED_ASSERTION 134
#define EXIT_TOO_MUCH_MEMORY 137
#define SEGMENTATION_FAULT 139

typedef uint8_t u8;

struct Shell {
		const char* Cursor = "$";
		char* CommandBuf;
} Shell;

void test_fprintf(FILE *stream, const char* format, va_list args)
{
		char* test_msg = (char*)"[xtraTest] ";
		strcat(test_msg, format);
		fprintf(stream, test_msg, args);
}

void test_run_all(void)
{
}

pid_t call(char* argv[])
{
		pid_t pid = fork();
		if(pid==-1)
		{
				exit(EXIT_FAILURE);
		}
		else if(pid==0)
		{
				char* envp[] = {NULL};

				execve(argv[0], argv, envp);

				perror("Could not execve\n");
				exit(EXIT_FAILURE);
		}
		else if(pid > 0)
		{
				fprintf(stdout, "im the parent\n");
		}
		else
		{

				fprintf(stdout, "Sub program started with args: %s\n", argv[0]);
				return pid;
		}
}

void sgets(char* str, int n)
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

//int main(int argc, char* argv[])
int main()
{
		pid_t pid;
    
		static char *newargv[] = { NULL, "hello", "world", NULL };
		char* argv[4];
		Shell.CommandBuf = (char*)malloc(sizeof(char) * 2);

		for(;;)
		{
				fprintf(stdout, "%s ", Shell.Cursor);
				sgets(Shell.CommandBuf, 128);
				execve(argv[0], newargv, NULL);
				perror("execve error");
				exit(EXIT_FAILURE);

				//pid = call(argv);
				//fprintf(stdout, "New process git pid: %d\n", pid);
		}

		return EXIT_SUCCESS;
}
