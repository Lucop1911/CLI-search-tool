#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_PATH 4096
#define MAX_LINE 1024

typedef struct {
    int search_contents;
    int search_filenames;
    int search_folders;
    int case_sensitive;
    int recursive;
    char *search_term;
    char *start_path;
} SearchOptions;

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] <search_term> [path]\n\n", program_name);
    printf("Search for a word or phrase in files, filenames, or folder names.\n\n");
    printf("OPTIONS:\n");
    printf("  -c    Search in file contents (default)\n");
    printf("  -f    Search in filenames\n");
    printf("  -d    Search in directory/folder names\n");
    printf("  -a    Search in all (contents, filenames, and folders)\n");
    printf("  -i    Case-insensitive search\n");
    printf("  -r    Recursive search through subdirectories\n");
    printf("  -h    Show this help message\n\n");
    printf("EXAMPLES:\n");
    printf("  %s -c \"hello world\" .          # Search for 'hello world' in file contents\n", program_name);
    printf("  %s -f main .                   # Search for 'main' in filenames\n", program_name);
    printf("  %s -a -i -r test /home/user    # Search everywhere for 'test' (case-insensitive, recursive)\n", program_name);
}

char *to_lowercase(const char *str) {
    char *lower = malloc(strlen(str) + 1);
    if (!lower) return NULL;
    
    for (int i = 0; str[i]; i++) {
        lower[i] = tolower(str[i]);
    }
    lower[strlen(str)] = '\0';
    return lower;
}

int contains_term(const char *text, const char *term, int case_sensitive) {
    if (case_sensitive) {
        return strstr(text, term) != NULL;
    } else {
        char *text_lower = to_lowercase(text);
        char *term_lower = to_lowercase(term);
        int found = strstr(text_lower, term_lower) != NULL;
        free(text_lower);
        free(term_lower);
        return found;
    }
}

void search_file_contents(const char *filepath, const char *term, int case_sensitive) {
    FILE *file = fopen(filepath, "r");
    if (!file) return;
    
    char line[MAX_LINE];
    int line_num = 0;
    int found = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        if (contains_term(line, term, case_sensitive)) {
            if (!found) {
                printf("\n\033[1;32m%s\033[0m\n", filepath);
                found = 1;
            }
            // Remove newline for display
            line[strcspn(line, "\n")] = 0;
            printf("  \033[1;33mLine %d:\033[0m %s\n", line_num, line);
        }
    }
    
    fclose(file);
}

void search_directory(const char *path, SearchOptions *opts) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char fullpath[MAX_PATH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        
        struct stat statbuf;
        if (stat(fullpath, &statbuf) == -1) {
            continue;
        }
        
        if (S_ISDIR(statbuf.st_mode)) {
            // Check folder name
            if (opts->search_folders && contains_term(entry->d_name, opts->search_term, opts->case_sensitive)) {
                printf("\n\033[1;34m[FOLDER]\033[0m %s\n", fullpath);
            }
            
            // Recursive search
            if (opts->recursive) {
                search_directory(fullpath, opts);
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            // Check filename
            if (opts->search_filenames && contains_term(entry->d_name, opts->search_term, opts->case_sensitive)) {
                printf("\n\033[1;36m[FILE]\033[0m %s\n", fullpath);
            }
            
            // Search file contents
            if (opts->search_contents) {
                search_file_contents(fullpath, opts->search_term, opts->case_sensitive);
            }
        }
    }
    
    closedir(dir);
}

int main(int argc, char *argv[]) {
    SearchOptions opts = {
        .search_contents = 0,
        .search_filenames = 0,
        .search_folders = 0,
        .case_sensitive = 1,
        .recursive = 0,
        .search_term = NULL,
        .start_path = "."
    };
    
    int opt;
    while ((opt = getopt(argc, argv, "cfdarih")) != -1) {
        switch (opt) {
            case 'c':
                opts.search_contents = 1;
                break;
            case 'f':
                opts.search_filenames = 1;
                break;
            case 'd':
                opts.search_folders = 1;
                break;
            case 'a':
                opts.search_contents = 1;
                opts.search_filenames = 1;
                opts.search_folders = 1;
                break;
            case 'r':
                opts.recursive = 1;
                break;
            case 'i':
                opts.case_sensitive = 0;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Get search term
    if (optind >= argc) {
        fprintf(stderr, "Error: Missing search term\n\n");
        print_usage(argv[0]);
        return 1;
    }
    opts.search_term = argv[optind];
    
    // Get start path if provided
    if (optind + 1 < argc) {
        opts.start_path = argv[optind + 1];
    }
    
    // Default to searching contents if nothing specified
    if (!opts.search_contents && !opts.search_filenames && !opts.search_folders) {
        opts.search_contents = 1;
    }
    
    printf("\033[1mSearching for: \033[0m'%s'\n", opts.search_term);
    printf("\033[1mIn path: \033[0m%s\n", opts.start_path);
    printf("\033[1mMode: \033[0m%s%s%s | %s | %s\n",
           opts.search_contents ? "Contents " : "",
           opts.search_filenames ? "Filenames " : "",
           opts.search_folders ? "Folders " : "",
           opts.case_sensitive ? "Case-sensitive" : "Case-insensitive",
           opts.recursive ? "Recursive" : "Non-recursive");
    printf("----------------------------------------\n");
    
    search_directory(opts.start_path, &opts);
    
    printf("\n\033[1mSearch complete.\033[0m\n");
    return 0;
}