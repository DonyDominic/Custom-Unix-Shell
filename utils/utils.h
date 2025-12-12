#ifndef UTILS_H
#define UTILS_H

#define PATH_MAX 4096 /* # chars in a path name including nul */

// Define a structure to hold all system information
typedef struct {
    char username[256];
    char pc_name[256];
    char dir_name[256]; // Just the last directory name
} SystemInfo;

// Function declaration:
// This function will fetch all system information and populate the SystemInfo struct.
// It returns 0 on success, -1 on failure.
int get_system_info(SystemInfo *info);
int display_custom_prompt(SystemInfo *info);

#endif // UTILS_H