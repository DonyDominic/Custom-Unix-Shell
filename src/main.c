/* gcc src/main.c src/parser.c src/tokens.c src/exec.c -Iinclude -o main && ./main */
#include "terminal.h"
int main()
{

    SystemInfo info;

    if(get_system_info(&info) == -1){
        perror("Failed to retrive user database");
    }
    char text_buffer[1024]; // store the cmds
    int status; // program status

    ssize_t bytes_read; // total lenght of the cmds
    bool running = true;

    while (running)
    {
        // write(STDOUT_FILENO, "$ ", 2);
        if(display_custom_prompt(&info) == -1){
            perror("Failed to display user info");
        }

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

        // skip empty commands
        if (strlen(text_buffer) == 0)
        {
            continue;
        }

        // quit program on `exit`
        if (strcmp(text_buffer, "exit") == 0)
        {
            running = false;
        }

        else
        {
            int token_count = 0;

            Token **tokens = lexer(text_buffer, &token_count);
            // print_tokens(tokens);
            cmd_node *tree = parse_tokens(tokens, 0, token_count - 1);
            free_tokens(tokens);
            status = execute_tree(tree);
            // free_tree(tree);
        }
    }
    return status;
}