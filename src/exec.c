#include "terminal.h"

int execute_simple_cmd(cmd_node *node);
int execute_pipe(cmd_node *node);
int execute_redirin(cmd_node *node);
int execute_redirout(cmd_node *node);
int execute_redirappend(cmd_node *node);
int execute_tree(cmd_node *node);
int is_builtin(char *cmd);
int execute_builtin(cmd_node *node);

/**
 * @brief to check whether a command is buildin
 * @param cmd command
 * 
 * @return 1 is true else 0
 */
int is_builtin(char *cmd)
{
    if (cmd == NULL)
        return 0;
    if (strcmp(cmd, "cd") == 0)
        return 1;
    if (strcmp(cmd, "exit") == 0)
        return 1;
    if (strcmp(cmd, "pwd") == 0)
        return 1;
    return 0;
}

/**
 * @brief execute buildin command in the parent process
 * @param node command node
 * @return 0 on success , -1 on err
 */
int execute_builtin(cmd_node *node)
{
    if (strcmp(node->argv[0], "cd") == 0)
    {
        if (node->argv[1] == NULL)
        {
            fprintf(stderr, "cd: expected argument\n");
        }
        else
        {
            if (chdir(node->argv[1]) != 0)
            {
                perror("cd");
            }
        }
        return 0;
    }

    if (strcmp(node->argv[0], "exit") == 0)
    {
        exit(0);
    }

    if (strcmp(node->argv[0], "pwd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        return 0;
    }
    return -1;
}

/**
    @brief  execute an `simple cmd `
    @param node  a `cmd` node
    @return 0 on success and -1 on error
*/
int execute_simple_cmd(cmd_node *node)
{
    if (node == NULL)
        return 0;
    else if (!node->argv || !node->argv[0])
    {
        return 0;
    }

    if (is_builtin(node->argv[0]))
    {
        return execute_builtin(node); // Runs in Parent, NO FORK
    }

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
            exit(-1);
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
 * @brief Excecute pipe command
 * @param node the pipe node
 *
 * @return 0 on success and -1 on err
 */
int execute_pipe(cmd_node *node)
{
    if (!node)
        return 0;
    if (!node->left || !node->right)
    {
        perror("| : expected two cmds");
        return -1;
    }
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
        // Left Child
        if (node->left->type == NODE_CMD)
        {
            execvp(node->left->argv[0], node->left->argv);
            perror("execvp");
            exit(-1);
        }
        else
        {
            status_l = execute_tree(node->left);
            exit(status_l);
        }
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
            exit(-1);
        }
        close(pipe_fds[0]);
        close(pipe_fds[1]);

        // Right Child
        if (node->right->type == NODE_CMD)
        {
            execvp(node->right->argv[0], node->right->argv);
            perror("execvp");
            exit(1);
        }
        else
        {
            status_r = execute_tree(node->right);
            exit(status_r);
        }
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
 * @brief Execute redirection in operator
 * @param node the redirection in node
 * @return 0 on success and -1 on err
 */
int execute_redirin(cmd_node *node)
{
    int status = 0;
    if (!node->right || !node->right->argv || !node->right->argv[0])
    {
        fprintf(stderr, "syntax error: expected file after redirection\n");
        return -1;
    }
    int fd = open(node->right->argv[0], O_RDONLY, 0);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    __pid_t pid = fork();
    if (pid == -1)
    {
        perror("FORK");
        return -1;
    }
    else if (pid == 0)
    {
        if (dup2(fd, STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(-1);
        }
        close(fd);
        if (node->left->type == NODE_CMD)
        {
            execvp(node->left->argv[0], node->left->argv);
            // If we reach this line, execvp FAILED
            perror(node->left->argv[0]);
            exit(-1);
        }
        exit(execute_tree(node->left));
    }
    else
    {
        close(fd);
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

/**
 * @brief Execute redirection out operator
 * @param node the redirection out node
 * @return 0 on success and -1 on err
 */
int execute_redirout(cmd_node *node)
{
    int status = 0;
    if (!node->right || !node->right->argv || !node->right->argv[0])
    {
        fprintf(stderr, "syntax error: expected file after redirection\n");
        return -1;
    }
    int fd = open(node->right->argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    __pid_t pid = fork();
    if (pid == -1)
    {
        perror("FORK");
        return -1;
    }
    else if (pid == 0)
    {
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(-1);
        }
        close(fd);
        if (node->left->type == NODE_CMD)
        {
            execvp(node->left->argv[0], node->left->argv);
            // If we reach this line, execvp FAILED
            perror(node->left->argv[0]);
            exit(-1);
        }
        exit(execute_tree(node->left));
    }
    else
    {
        close(fd);
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

/**
 * @brief Execute redirection append operator
 * @param node the redirection append node
 * @return 0 on success and -1 on err
 */
int execute_redirappend(cmd_node *node)
{
    int status = 0;
    if (!node->right || !node->right->argv || !node->right->argv[0])
    {
        fprintf(stderr, "syntax error: expected file after redirection\n");
        return -1;
    }
    int fd = open(node->right->argv[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    __pid_t pid = fork();
    if (pid == -1)
    {
        perror("FORK");
        return -1;
    }
    else if (pid == 0)
    {
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(-1);
        }
        close(fd);
        if (node->left->type == NODE_CMD)
        {
            execvp(node->left->argv[0], node->left->argv);
            // If we reach this line, execvp FAILED
            perror(node->left->argv[0]);
            exit(-1);
        }
        exit(execute_tree(node->left));
    }
    else
    {
        close(fd);
        waitpid(pid, &status, 0);
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
    if (node == NULL)
        return 0;

    int status = 0;
    switch (node->type)
    {
    case NODE_SEMICOLON:
        // do left and right, independently
        status = execute_tree(node->left);
        return execute_tree(node->right);
    case NODE_AND:
        // do left and if it succeeds do right
        status = execute_tree(node->left);
        if (status == 0)
        {
            return execute_tree(node->right);
        }
        else
        {
            return status;
        }
    case NODE_OR:
        // do left, and if it fails only do right
        status = execute_tree(node->left);
        if (status != 0)
        {
            return execute_tree(node->right);
        }
        return status;
    case NODE_PIPE:
        return execute_pipe(node);
    case NODE_REDIR_IN:
        return execute_redirin(node);
    case NODE_REDIR_OUT:
        return execute_redirout(node);
    case NODE_REDIR_APPEND:
        return execute_redirappend(node);
    case NODE_CMD:
        return execute_simple_cmd(node);
    default:
        printf("No vaild token found\n");
        break;
    }

    return 0;
}
