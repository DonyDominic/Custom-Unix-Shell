/*
parser.c
-includes functions to parse prompt into tokens, based on delimators and escape sequences
*/

#include "terminal.h"

// function parse_command(command_string):
// 1. Check for Level 1 (&&, ||) - Start here
//    - Scan RTL for these operators. If found, split and recurse (left and right children).

// 2. Check for Level 2 (;)
//    - Scan RTL. If found, split and recurse.

// 3. Check for Level 3 (|)
//    - Scan RTL. If found, split and recurse.

// 4. Check for Level 4 (&)
//    - Check for trailing &. If found, split the command part and set the background flag.

// 5. Base Case / Command Node (Level 5)
//    - If no lower-priority operators were found, this is a single command.
//    - Scan LTR for <, >, >> and extract the file arguments.
//    - Create the final Command Node.

// return tree_node


// basic for now
int parse_by_delm(char *data, char *delm, char **token_buffer)
{
    char *ptr;
    int n_tok = 0;
    char *token = __strtok_r(data, delm, &ptr);
    while (token != NULL)
    {
        // printf("%s\n", token);
        token_buffer[n_tok] = strdup(token);

        n_tok++;
        token = __strtok_r(NULL, delm, &ptr);
    }
    return n_tok;
}

void print_token_buffer(char** token_buffer,size_t n_tokens){
    for (size_t i = 0; i < n_tokens; i++)
    {
        printf("%s\n",token_buffer[i]);
    }
    
}