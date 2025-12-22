
#if !defined(PARSER)
#define PARSER

/**
 * @brief a collection of vaild node types.
 */
typedef enum{
    // 1
    NODE_SEMICOLON, // ;
    NODE_BACKGROUND,   // &
    NODE_AND,      // &&
    NODE_OR,       // ||
    // 2
    NODE_PIPE,         // |
    // 3
    NODE_REDIR_IN,     // <
    NODE_REDIR_OUT,    // >
    NODE_REDIR_APPEND, // >>
    // 4
    NODE_CMD,
} Node_type;

/**
 * @brief a node that store information about a `cmd` or `operator` in the `tree`
 */
typedef struct cmd_node
{
    Node_type type; // type of node

    struct cmd_node* left; // left child
    struct cmd_node* right; // right child
    char** argv; // `cmd` arguments (if any) 
    
}cmd_node;

cmd_node* parse_tokens(Token** tokens,int start,int end);
int free_node(cmd_node* node);


#endif // PARSER
