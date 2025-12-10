#include "terminal.h"
#include <sys/wait.h>
/*
    Execute an `simple cmd `
*/
int execute_simple_cmd(Command_Node *node)
{
    __pid_t pid = fork();
    int status,wait_result;
    // child
    if (pid == -1)
    {
        perror("FORK");
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {

        if (execvp(node->argv[0], node->argv) == -1)
        {
            perror(node->argv[0]);
            return EXIT_FAILURE;
        }
    }

    // parent
    else
    {
        do
        {
            // waitpid will block until the child terminates
            wait_result = waitpid(pid, &status, 0);
        } while (wait_result == -1); // Handle interruptions

        // 2. Extract the exit status
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status); // Returns 0-255
        }
        else if (WIFSIGNALED(status))
        {
            // Child was terminated by a signal (e.g., Ctrl+C, kill -9)
            return 128 + WTERMSIG(status);
        }
        // Handle other cases (stopped, continued, etc.) if needed.
        return 1; // Default error status if termination is unusual
    }
}