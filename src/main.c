/* gcc src/main.c src/parser.c src/tokens.c src/exec.c -Iinclude -o main && ./main */
#include "terminal.h"
int main()
{

    SystemInfo info;
    get_system_info(&info);
    char text_buffer[1024]; // store the cmds

    ssize_t bytes_read; // total lenght of the cmds
    bool running = true;

    while (running)
    {
        // write(STDOUT_FILENO, "$ ", 2);
        display_custom_prompt(&info);

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

           Token** tokens = lexer(text_buffer);
           print_tokens(tokens); 

        }
    }
    return 0;
}