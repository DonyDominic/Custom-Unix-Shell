#include "terminal.h"

cmd_node *parse_tokens(Token **tokens, int start, int end);
cmd_node *create_simple_cmd_node(Token **tokens, int start, int end);

/**
 * @brief create a node with sequence (;) operator as parent.
 *
 * @param tokens   the token list
 * @param start   start index of the token
 * @param split_index   the index at which the node to be split
 * @param end  end index of the token
 *
 * @return `cmd_node*` on success and NULL on error
 */
cmd_node *create_sequence_node(Token **tokens, int start, int split_index, int end)
{
    cmd_node *node = (cmd_node *)malloc(sizeof(cmd_node));

    if (!node)
    {
        perror("malloc : node(*cmd_node)\n");
        return NULL;
    }
    // assigning `;` as the parent
    node->type = tokens[split_index]->type;

    // `;` has no meta-data
    node->argv = NULL;
    node->input_file = NULL;
    node->output_file = NULL;

    // recursively parsing left and right child
    node->left = parse_tokens(tokens, start, split_index - 1);
    node->right = parse_tokens(tokens, split_index + 1, end);

    return node;
}

/**
 * @brief  recursively scans tokens backwads and append them into `tree` nodes.
 *
 * @param tokens  the token list
 * @param start  start index of the token list
 * @param end  end index of the token list
 *
 * @return `cmd_node*` on success and NULL on error.
 */
cmd_node *parse_tokens(Token **tokens, int start, int end)
{

    // scan backwards

    for (size_t i = end; i > start; i--)
    {
        if (tokens[i]->type == TOKEN_SEMICOLON)
        {
            cmd_node *node = create_sequence_node(tokens, start, i, end);
            return node;
        }
    }
    return create_simple_cmd_node(tokens, start, end);
}

/**
 * @brief create a simple cmd node
 *
 * @param tokens  the token list
 * @param start  start index of the token lsit
 * @param end  end index of the token list
 * 
 * @return `cmd_node*` on success and NULL on error.
 */
cmd_node *create_simple_cmd_node(Token **tokens, int start, int end)
{
    cmd_node *node = (cmd_node *)malloc(sizeof(cmd_node));
    if (!node)
    {
        perror("malloc : node(*cmd_node)\n");
        return NULL;
    }
    node->type = NODE_CMD;
    node->left = NULL;
    node->right = NULL;

    // --- Allocate and populate argv array ---
    int num_args = end - start + 1;
    // We need space for all arguments + the NULL terminator
    node->argv = (char **)calloc(num_args + 1, sizeof(char *));

    int arg_index = 0;

    for (size_t i = start; i <= end; i++)
    {
        node->argv[arg_index] = strdup(tokens[i]->value);
        arg_index++;
    }
    node->argv[num_args] = NULL;
    return node;
}
