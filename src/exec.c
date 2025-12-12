#include "terminal.h"
#include <sys/wait.h>
/**
    @brief  execute an `simple cmd `
    @param node  a `cmd` node
    @return 0 on success and -1 on error
*/
int execute_simple_cmd(cmd_node *node)
{
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

/**
 * @brief exceute a `cmd_node` tree recursively
 * @param node  `cmd_node`
 * @return 0 on success and -1 on error
 */
int execute_tree(cmd_node *node)
{

    int status = 0;
    switch (node->type)
    {
    case NODE_SEMICOLON:
        status = execute_tree(node->left);
        return execute_tree(node->right);

    case NODE_CMD:
        return execute_simple_cmd(node);
    default:
        printf("No vaild token found\n");
        break;
    }

    return 0;
}