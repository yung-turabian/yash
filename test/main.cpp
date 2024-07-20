#include "../include/yungTest.h"

#include <cstdlib>
#include <cstdio>
#include <unistd.h>

int add(int a, int b) {
		return a + b;
}

int test_add() {
    ASSERT_EQ(4, add(2, 2));
    ASSERT_EQ(0, add(-2, 2));
    ASSERT_EQ(-4, add(-2, -2));
    return 0;
}

#define BUFFER_SIZE 1024

void run_test(const char *command) {
    int pipefd_in[2];  // Pipe for stdin
    int pipefd_out[2]; // Pipe for stdout
    pid_t pid;
    char buffer[BUFFER_SIZE];
    int status;

    if (pipe(pipefd_in) == -1) {
        perror("pipe in");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd_out) == -1) {
        perror("pipe out");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        close(pipefd_in[1]);  // Close write end of stdin pipe
        dup2(pipefd_in[0], STDIN_FILENO);  // Redirect stdin to read end of pipe
        close(pipefd_in[0]);

        close(pipefd_out[0]); // Close read end of stdout pipe
        dup2(pipefd_out[1], STDOUT_FILENO); // Redirect stdout to write end of pipe
        close(pipefd_out[1]);

        // Replace the child process with your shell
        execl("./hrry", "./hrry", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } 
		else {  // Parent process
        close(pipefd_in[0]);  // Close read end of stdin pipe
        close(pipefd_out[1]); // Close write end of stdout pipe

        // Write command to shell's stdin
        if (write(pipefd_in[1], command, strlen(command)) == -1) {
            perror("write command");
        }
        if (write(pipefd_in[1], "\n", 1) == -1) {
            perror("write newline");
        }
        close(pipefd_in[1]);  // Close write end to signal EOF

        // Read the shell's output
        int bytes_read;
        while ((bytes_read = read(pipefd_out[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);  // Print the shell's output
        }
        if (bytes_read == -1) {
            perror("read");
        }

        close(pipefd_out[0]); // Close read end of stdout pipe

        // Wait for the child process to finish
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
    }
}

int main() {
		//RUN_TEST(test_add);


		run_test("exit");


		run_test("help");


		run_test("ls -l");

}
