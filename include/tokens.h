
// operator token type wrt priority (lowest->highest)
typedef enum{
    // 1
    TOKEN_DEL_AND,      // &&
    TOKEN_DEL_OR,       // ||
    TOKEN_DEL_SEQUENCE, // ;
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

struct cmd_node;

typedef struct redir
{
    char* filename;
    Token_type type; // REDIR_IN, REDIR_OUT, etc.
} Redir;

typedef struct cmd_node
{
    Token_type type; 

    // Used if type is an OPERATOR 
    // The recursive result of parsing the left and right halves.
    struct cmd_node* left; 
    struct cmd_node* right;

    // Used if type is TOKEN_CMD (The Leaf Node)
    // The actual arguments for execve(). A NULL-terminated array of strings.
    char** argv; 
    int argc;

    // Redirections associated with this command (e.g., cmd < file > out)
    Redir* redirs;
    int num_redirs;
    
    // Flag for background execution (cmd &)
    int background; 
    
} Command_Node;

Command_Node* simple_cmd(char** token_buffer,int n_tokens);