#include <stdio.h>
#include "string.h"

#ifdef _WIN32
#include <direct.h>
#define popen _popen
#define pclose _pclose
#define getcwd _getcwd
#define MAKE_COMPILE "mingw32-make compile"
#else
#include <unistd.h>
#define MAKE_COMPILE "make compile"
#endif


int main(void) {
    FILE *command_fd = popen(MAKE_COMPILE, "r");
    if (!command_fd) {
        perror("popen");
        return 1;
    }

    char last_line[512];

    while (fgets(last_line, sizeof(last_line), command_fd)) {}
    printf("output: %s", last_line);

    int status = pclose(command_fd);

    FILE *out_fd = fopen("compile_commands.json", "w");

    if (!out_fd) {
        perror("could not open compile_commands.json file for writing");
        return 1;
    }

    fprintf(out_fd, "[{\"directory\":");

    char current_directory[1024];
    
    if (getcwd(current_directory, sizeof(current_directory)) != NULL) {
        fprintf(out_fd, "\"");
        char* token = strtok(current_directory, "\\");
        while (token != NULL) {
            fprintf(out_fd, "%s", token);
            token = strtok(NULL, "\\");
            if (token != NULL) {
                fprintf(out_fd, "\\\\");
            }
        }
        fprintf(out_fd, "\",");
    } else {
        perror("getcwd failed.");
        return 1;
    }
    
    fprintf(out_fd, "\"arguments\": [");

    char* token = strtok(last_line, " \n");

    while (token != NULL) {
        fprintf(out_fd, "\"%s\"", token);
        token = strtok(NULL, " \n");
        if (token != NULL) {
            fprintf(out_fd, ",");
        }
    }
    fprintf(out_fd, "],");
    fprintf(out_fd, "\"file\": \"main.c\"}]");

    fclose(out_fd);

    return 0;
}
