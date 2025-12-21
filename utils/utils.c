
#include "terminal.h"

#define PATH_SEP '/'
#define PATH_MAX 4096 /* # chars in a path name including nul */

/**
 * @brief Fetchs user information from the user database
 * @param info A pointer to the SystemInfo struct containing current system data.
 * @return 0 on success, -1 on failure.
 */
int get_system_info(SystemInfo *info)
{
    if (info == NULL)
        return -1; // Basic safety check

    // Username
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw != NULL)
    {
        strncpy(info->username, pw->pw_name, sizeof(info->username) - 1);
    }
    else
    {
        strncpy(info->username, "unknown_user", sizeof(info->username) - 1);
    }

    // 2. PC Name (Hostname)
    if (gethostname(info->pc_name, sizeof(info->pc_name)) != 0)
    {
        strncpy(info->pc_name, "unknown_host", sizeof(info->pc_name) - 1);
    }

    // cwd
    char full_path[PATH_MAX]; // PATH_MAX provides a safe maximum buffer size

    if (getcwd(full_path, sizeof(full_path)) == NULL)
    {
        perror("Error retrieving CWD");
        return -1;
    }

    /*Extract `cwd` from `full_path`*/
    // find last `/` in full_path
    char *last_slash = strrchr(full_path, PATH_SEP);
    // Extract the dir name
    if (last_slash != NULL)
    {
        // If the path is "/", last_slash points to the start of the string.
        // If the path is "/home/user", last_slash points to the '/' before "user".

        // move one character past the last '/' to get the directory name.
        char *directory_name = last_slash + 1;

        // Handle the special case where CWD is root directory ("/").
        // In this case, directory_name points to the NULL terminator, resulting in ""
        // If the path is "/", we want to return "/" itself.
        if (strcmp(directory_name, "") == 0 && last_slash == full_path)
        {
            strncpy(info->dir_name, "/", sizeof(info->dir_name) - 1);
        }
        else
        {
            // dir_name is saved
            strncpy(info->dir_name, directory_name, sizeof(info->dir_name) - 1);
        }
    }
    else
    {
        printf("Last CWD Component: Error (No path separator found)\n");
    }
    return 0;
}

// Max length for a prompt string.
#define PROMPT_BUFFER_SIZE 512

/**
 * @brief writes the custom shell prompt to STDOUT.
 * @param info a pointer to the SystemInfo struct containing current system data.
 * @return 0 on success, -1 on failure.
 */
int display_custom_prompt(SystemInfo *info)
{
    char prompt_buffer[PROMPT_BUFFER_SIZE];
    int len;

    // 1. Update system information (CWD might have changed since the last command)
    if (get_system_info(info) != 0)
    {
        // Fallback prompt if info retrieval fails
        write(STDOUT_FILENO, "shell$ ", 7);
        return -1;
    }

    // snprintf returns the number of characters *that would have been* written,
    // excluding the null terminator.
    len = snprintf(
        prompt_buffer,
        PROMPT_BUFFER_SIZE,
        "[%s@%s %s]$  ",
        info->username,
        info->pc_name,
        info->dir_name);

    // Check if the prompt was truncated (len >= PROMPT_BUFFER_SIZE)
    if (len >= PROMPT_BUFFER_SIZE || len < 0)
    {
        // If truncation or error occurs, write a simple fallback prompt
        write(STDOUT_FILENO, "shell$ ", 8);
        return -1;
    }

    // 3. Write the resulting string to the terminal
    write(STDOUT_FILENO, prompt_buffer, len);

    return 0;
}