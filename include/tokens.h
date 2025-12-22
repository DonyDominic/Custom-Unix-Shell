/**
 * @brief states of the tokenizer. 
 */
typedef enum{
    IS_SPACE,
    IS_WORD
} state;

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
 * helper struct for mapping a operator(char) to `Token_type`
 */
typedef struct {
    Token_type type;
    int len;
} OpInfo;


/**
 * @brief `token`- `cmd` pair used in `lexer`
 */
typedef struct {
    Token_type type;
    char *value;
} Token;


Token **lexer(const char *input,int* token_count);
void free_tokens(Token** tokens);
