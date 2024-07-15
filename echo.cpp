#include <cstdio>
#include <cstdlib>

int
main(int argc, char* argv[])
{
		int i;
		for(i=1;i<argc;i++)
		{
				fprintf(stdout, "%s ", argv[i]);
		}
		fprintf(stdout, "\n");

		exit(EXIT_SUCCESS);
}
