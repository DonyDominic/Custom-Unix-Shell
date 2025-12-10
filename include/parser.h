int parse_by_delm(char *text_buffer, char *delm, char **token_buffer);
void print_token_buffer(char** token_buffer,size_t n_tokens);


// simple state machine

typedef enum{
    IN_CMD,
    IN_WS, // white space
    IN_SQ, // single quotes
    IN_DQ, // double quotes
    OP
}STATE;

// char delms = {"&&","||",";","|","&","<",">",">>"};
