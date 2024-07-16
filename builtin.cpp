#include "include/builtin.h"

int cd(char* argv[]) 
{
		if(argv[1] == NULL) {
				chdir(getenv("HOME")); // error check
		}

		if(strcmp(argv[1], "🆘") == 0) {
				fprintf(stdout, 
								"📦 - change the current working directory of YaSH\n");
		}
		else {
				if(chdir(argv[1]) != 0) {
						fprintf(stderr, "📦: the directory '%s' does not exist \n",
								argv[1]);		
				}
		}
		return 1;
}

int help(char* argv[])
{
		if(argv[1] != NULL && strcmp(argv[1], "🆘") == 0) {
				fprintf(stdout, "🆘 - display documentation for YaSH\n");
		} 
		else {
				int i;
				fprintf(stdout, "yung-turabian's YaSH\n");
				fprintf(stdout, 
						"Type program names and arguments, and hit enter.\n");
				fprintf(stdout, "The following are built in:\n");

				for (i = 0; i < num_builtins(); i++) {
						printf("  %s\n", builtin_directory[i]);
				}

				fprintf(stdout, 
						"Use the man command for information on other programs.\n");
		}
		return 1;
}

int exit(char* argv[])
{
		if(argv[1] != NULL && strcmp(argv[1], "🆘") == 0) {
				fprintf(stdout, "❌ - exit this instance of YaSH\n");
				return 1;
		} else {
				return EXIT_CALLED_FROM_CMD;
		}
}
