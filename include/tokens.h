
// operator token type wrt priority (lowest->highest)

/**
 * @brief a collection of vaild token types.
 */
typedef enum{
    // 1
    TOKEN_SEMICOLON, // ;
    TOKEN_BACKGROUND,   // &
    TOKEN_AND,      // &&
    TOKEN_OR,       // ||
    // 2
    TOKEN_PIPE,         // |
    // 3
    TOKEN_REDIR_IN,     // <
    TOKEN_REDIR_OUT,    // >
    TOKEN_REDIR_APPEND, // >>
    // 4
    TOKEN_CMD,
    TOKEN_END,
} Token_type;

/**
 * @brief `token`- `cmd` pair used in `lexer`
 */
typedef struct {
    Token_type type;
    char *value;
} Token;


Token **lexer(const char *input,int* token_count);
void free_tokens(Token** tokens);
void print_tokens(Token** tokens);