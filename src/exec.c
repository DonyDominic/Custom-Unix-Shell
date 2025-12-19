#include "terminal.h"
#include <sys/wait.h>
/**
    @brief  execute an `simple cmd `
    @param node  a `cmd` node
    @return 0 on success and -1 on error
*/
int execute_simple_cmd(cmd_node *node)
{
    if (node == NULL)
        return 0;
    __pid_t pid = fork();
    int status;
    // child
    if (pid == -1)
    {
        perror("FORK");
        return -1;
    }
    else if (pid == 0)
    {

        if (execvp(node->argv[0], node->argv) == -1)
        {
            perror(node->argv[0]);
            return -1;
        }
    }

    // parent
    else
    {
        // Wait for the child process to finish and get its status
        waitpid(pid, &status, 0);

        // Return the actual exit status of the child
        return WEXITSTATUS(status);
    }
}

int execute_pipe(cmd_node *node)
{
    int pipe_fds[2];
    int status_l, status_r;
    if (pipe(pipe_fds) == -1)
    {
        perror("pipe");
        return -1;
    }
    __pid_t pid_l = fork();
    // left child
    if (pid_l == -1)
    {
        perror("FORK");
        return -1;
    }
    else if (pid_l == 0)
    {
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            return -1;
        }
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        status_l = execute_tree(node->left);
        exit(status_l);
    }

    __pid_t pid_r = fork();
    // right child
    if (pid_r == -1)
    {
        perror("FORK");
        return -1;
    }
    else if (pid_r == 0)
    {
        if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            return -1;
        }
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        status_r = execute_tree(node->right);
        exit(status_r);
    }

    // parent
    // Parent closes BOTH ends so children get EOF
    close(pipe_fds[0]);
    close(pipe_fds[1]);

    // Parent waits for both children
    waitpid(pid_l, &status_l, 0);
    waitpid(pid_r, &status_r, 0);

    return WEXITSTATUS(status_r);
}
/**
 * @brief exceute a `cmd_node` tree recursively
 * @param node  `cmd_node`
 * @return 0 on success and -1 on error
 */
int execute_tree(cmd_node *node)
{
    if (node == NULL)
        return 0;

    int status = 0;
    switch (node->type)
    {
    case NODE_SEMICOLON:
        status = execute_tree(node->left);
        return execute_tree(node->right);

    case NODE_PIPE:
        return execute_pipe(node);
    case NODE_CMD:
        return execute_simple_cmd(node);
    default:
        printf("No vaild token found\n");
        break;
    }

    return 0;
}

