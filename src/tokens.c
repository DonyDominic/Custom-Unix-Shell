#include "terminal.h"

// Max number of tokens expected
#define MAX_TOKENS 128
// Maximum length of any single word/token
#define MAX_TOKEN_LEN 64

OpInfo get_op_info(const char *p);
void flush_buffer(Token **tokens, int *token_count, char *buffer, int *buff_len);
void handle_quoted_content(const char **p, char *buffer, int *buff_len);
Token *create_token(Token_type type, const char *start, int length);
Token **lexer(const char *input, int *token_count);
void free_tokens(Token **tokens);

/**
 * @brief helper function for indentifing operator's.
 * @param p character to compare
 * 
 * @return OpInfo
 */
OpInfo get_op_info(const char *p) {
    if (*p == '|' && *(p+1) == '|') return (OpInfo){TOKEN_OR, 2};
    if (*p == '&' && *(p+1) == '&') return (OpInfo){TOKEN_AND, 2};
    if (*p == '>' && *(p+1) == '>') return (OpInfo){TOKEN_REDIR_APPEND, 2};
    
    if (*p == '|') return (OpInfo){TOKEN_PIPE, 1};
    if (*p == '&') return (OpInfo){TOKEN_BACKGROUND, 1};
    if (*p == ';') return (OpInfo){TOKEN_SEMICOLON, 1};
    if (*p == '<') return (OpInfo){TOKEN_REDIR_IN, 1};
    if (*p == '>') return (OpInfo){TOKEN_REDIR_OUT, 1};
    
    return (OpInfo){-1, 0};
}

/**
 * @brief to create a new `token`
 *
 * @param type  type of token
 * @param start  start index of the token string
 * @param length  lenght of the string to be copied wrt `start`
 *
 * @return `Token*` on success and `NULL` on error.
 */

Token *create_token(Token_type type, const char *start, int length)
{
    Token *new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
    {
        perror("malloc : new_token(Token*)");
        return NULL;
    }
    new_token->type = type;
    new_token->value = strndup(start, length);
    if (!new_token->value)
    {
        free(new_token);
        perror("strndup : new_token->value");
        return NULL;
    }
    return new_token;
}

/**
 * @brief create a token with the current buffer.
 * 
 * @param tokens list of tokens
 * @param token_count lenght of token list
 * @param buffer temporary string buffer
 * @param buff_len lenght of the buffer
 */
void flush_buffer(Token **tokens, int *token_count, char *buffer, int *buff_len) {
    // Only create a token if there is actually something in the buffer
    if (*buff_len > 0) {
        buffer[*buff_len] = '\0'; // Always null-terminate before creating token
        
        // Use your existing create_token function
        tokens[*token_count] = create_token(TOKEN_CMD, buffer, *buff_len);
        
        if (tokens[*token_count] != NULL) {
            (*token_count)++;
        }
        
        *buff_len = 0; // Reset index for the next word
    }
}

/**
 * @brief eagar scan string from the input.
 * 
 * @param p quote in input
 * @param buffer temporary string buffer
 * @param buff_len lenght of the buffer
 */
void handle_quoted_content(const char **p, char *buffer, int *buff_len) {
    char quote_type = **p; // Store if it's ' or "
    (*p)++; // Skip the opening quote

    while (**p != '\0' && **p != quote_type) {
        if (*buff_len < MAX_TOKEN_LEN - 1) {
            buffer[*buff_len] = **p;
            (*buff_len)++;
        }
        (*p)++;
    }

    if (**p == quote_type) {
        (*p)++; // Skip the closing quote
    } else {
        fprintf(stderr, "Closing quote %c not found!\n", quote_type);
    }
}

/**
 * @brief convert`input` buffer into a `token` list
 *
 * @param input input buffer
 * @param token_count no of `tokens` found
 *
 * @return `tokens` on success and NULL on error
 */

Token **lexer(const char *input, int *token_count)
{
    Token **tokens = (Token **)malloc(MAX_TOKENS * sizeof(Token *));
    if (!tokens)
    {
        perror("malloc : tokens(Token**)");
        return NULL;
    }

    const char *p = input;
    char *buffer = malloc(MAX_TOKEN_LEN * sizeof(char));
    if(!buffer){
        perror("malloc : buffer(char*)");
        return NULL;
    }
    state state = IS_SPACE;
    int buff_len = 0;

    while (*p != '\0')
    {
        // Quotes (Eager)
        if (*p == '\'' || *p == '\"')
        {
            handle_quoted_content(&p, buffer, &buff_len);
            continue; 
        }

        OpInfo op = get_op_info(p);
        if (op.type != -1)
        {
            flush_buffer(tokens, token_count, buffer, &buff_len);
            tokens[(*token_count)++] = create_token(op.type, p, op.len);
            p += op.len;
            state = IS_SPACE;
            continue;
        }

        if (isspace(*p))
        {
            flush_buffer(tokens, token_count, buffer, &buff_len);
            state = IS_SPACE;
            p++;
            continue;
        }

        // Default: It's a word character
        buffer[buff_len++] = *p;
        state = IS_WORD;
        p++;
    }
    flush_buffer(tokens,token_count,buffer,&buff_len);
    tokens[*token_count] = create_token(TOKEN_END,"",0);
    free(buffer);
    return tokens;
}

/**
 * @brief free's `Token` and its subvalues.
 */
void free_tokens(Token **tokens)
{
    if (tokens)
    {
        size_t i;
        for (i = 0; tokens[i]->type != TOKEN_END; i++)
        {
            free(tokens[i]->value);
            free(tokens[i]);
        }

        // Free TOKEN_END
        free(tokens[i]->value);
        free(tokens[i]);
        free(tokens);
    }
    else
    {
        printf("No tokens found\n");
    }
}

