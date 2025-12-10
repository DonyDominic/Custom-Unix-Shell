#include "terminal.h"
/*
    Create a `token` for an simple cmd.
*/
Command_Node* simple_cmd(char** token_buffer,int n_tokens){
    Command_Node* node = calloc(1,sizeof(Command_Node));
    node->type = TOKEN_CMD;
    node->argc = n_tokens-1;
    node->argv = calloc(n_tokens+1,sizeof(char*));
    for (size_t i = 0; i < n_tokens; i++)
    {
        node->argv[i] = strdup(token_buffer[i]);
    }
    node->argv[n_tokens] = NULL;
    return node;
}


