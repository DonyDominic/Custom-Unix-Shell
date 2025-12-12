#ifndef UTILS_H
#define UTILS_H

#define PATH_MAX 4096 /* # chars in a path name including null */

// information on username,pc name and cwd
typedef struct {
    char username[256]; 
    char pc_name[256];
    char dir_name[256]; // Just the last directory name
} SystemInfo;


int get_system_info(SystemInfo *info);
int display_custom_prompt(SystemInfo *info);

#endif // UTILS_H