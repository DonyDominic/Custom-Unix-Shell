/* gcc src/main.c src/parser.c src/tokens.c src/exec.c -Iinclude -o main && ./main */
#include "terminal.h"
int main()
{

    char text_buffer[1024]; // store the cmds

    ssize_t bytes_read; // total lenght of the cmds
    bool running = true;

    while (running)
    {
        write(STDOUT_FILENO, "$ ", 2);

        bytes_read = read(STDIN_FILENO, text_buffer, sizeof(text_buffer) - 1);

        if (bytes_read < 0)
        {
            perror("stdin error");
            continue;
        }
        else if (bytes_read == 0)
        {
            write(STDOUT_FILENO, "\n", 1); // EOF
            break;
        }
        text_buffer[bytes_read] = '\0';
        // strcspn returns the 1st occurance of \n in buffer
        text_buffer[strcspn(text_buffer, "\n")] = '\0'; // ig newline is the end of cmd

        if (strlen(text_buffer) == 0)
        {
            // write(STDOUT_FILENO, "\n", 1); // EOF
            continue;
        }
        if (strcmp(text_buffer, "exit") == 0)
        {
            running = false;
        }

        else
        {
            // Token *token_list = tokenizer(text_buffer);
            // print_token_list(token_list);
            char **token_buffer = malloc(bytes_read);
            size_t n_tokens = parse_by_delm(text_buffer, DELIMATOR, token_buffer);
            // print_token_buffer(token_buffer,n_tokens);
            Command_Node *node = simple_cmd(token_buffer, n_tokens);
            execute_simple_cmd(node);
        }
    }
    return 0;
}