#include "terminal.h"

// Max number of tokens expected
#define MAX_TOKENS 128
// Maximum length of any single word/token
#define MAX_TOKEN_LEN 64

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

    while (*p != '\0')
    {
        while (isspace(*p))
            p++; // skip whitespaces

        if (*p == '\0')
            break;

        if (*p == '&' && *(p + 1) == '&')
        {
            tokens[(*token_count)++] = create_token(TOKEN_AND, p, 2); // copies addr p and p+1
            p += 2;
            continue;
        }
        if (*p == '|' && *(p + 1) == '|')
        {
            tokens[(*token_count)++] = create_token(TOKEN_OR, p, 2);
            p += 2;
            continue;
        }
        if (*p == '>' && *(p + 1) == '>')
        {
            tokens[(*token_count)++] = create_token(TOKEN_REDIR_APPEND, p, 2);
            p += 2;
            continue;
        }

        // Pipe |
        if (*p == '|')
        {
            tokens[(*token_count)++] = create_token(TOKEN_PIPE, p, 1);
            p += 1;
            continue;
        }
        // Sequence ;
        if (*p == ';')
        {
            tokens[(*token_count)++] = create_token(TOKEN_SEMICOLON, p, 1);
            p += 1;
            continue;
        }
        // Background &
        if (*p == '&')
        {
            tokens[(*token_count)++] = create_token(TOKEN_BACKGROUND, p, 1);
            p += 1;
            continue;
        }
        // Redirection Write >
        if (*p == '>')
        {
            tokens[(*token_count)++] = create_token(TOKEN_REDIR_OUT, p, 1);
            p += 1;
            continue;
        }
        // Redirection Read <
        if (*p == '<')
        {
            tokens[(*token_count)++] = create_token(TOKEN_REDIR_IN, p, 1);
            p += 1;
            continue;
        }

        // WORD SCANNER : takes a word and scans until its end and append it in the word buffer

        const char *start = p;
        while (*p != '\0' && !isspace(*p) &&
               *p != '|' && *p != '&' && *p != ';' && *p != '>' && *p != '<')
        {
            p++;
        }
        // We found the end of the word (at p). The word started at start.
        tokens[(*token_count)++] = create_token(TOKEN_CMD, start, p - start);
    }
    tokens[*token_count] = create_token(TOKEN_END, "", 0);

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

/**
 * @brief  print the `Token` list.
 */
void print_tokens(Token **tokens)
{
    if (tokens)
    {
        ssize_t i = 0;
        for (i = 0; tokens[i]->type != TOKEN_END; i++)
        {
            printf("%s\n", tokens[i]->value);
        }
    }
    else
    {
        printf("No tokens found\n");
    }
}