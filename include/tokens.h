
// operator token type wrt priority (lowest->highest)
typedef enum{
    // 1
    TOKEN_AND,      // &&
    TOKEN_OR,       // ||
    TOKEN_SEMICOLON, // ;
    // 2
    TOKEN_PIPE,         // |
    // 3
    TOKEN_BACKGROUND,   // &
    // 4
    TOKEN_REDIR_IN,     // <
    TOKEN_REDIR_OUT,    // >
    TOKEN_REDIR_APPEND, // >>
    // 5
    TOKEN_CMD,
    TOKEN_END,
} Token_type;

typedef struct {
    Token_type type;
    char *value;
} Token;

Token **lexer(const char *input);
void free_tokens(Token** tokens);
void print_tokens(Token** tokens);