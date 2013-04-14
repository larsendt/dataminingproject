#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <zlib.h>

#define PREFIX "/mnt/storage/data_mining_project/data/5grams/"
#define LINESIZE 4096

struct pathlist {
    char **paths;
    int count;
    int capacity;
};

void append_path(struct pathlist *paths, char *filename);
void free_paths(struct pathlist *paths);
int find_separator(char *str, char sep);

int main(int argc, char *argv[])
{
    struct pathlist paths;
    paths.count = 0;
    paths.capacity = 0;
    paths.paths = NULL;

    gzFile gzf;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(PREFIX)) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            append_path(&paths, ent->d_name);
        }
        closedir(dir);
    }
    else {
        perror("");
        return 1;
    }

    int i;
    char line[LINESIZE];
    char lastgram[LINESIZE];
    memset(lastgram, 0, LINESIZE);
    char *ptr;
    int sep;
    long long unsigned int unique_count = 0;
    for(i = 0; i < paths.count; i++) {
        gzf = gzopen(paths.paths[i], "rb");
        while((ptr = gzgets(gzf, line, LINESIZE)) != NULL) {
            sep = find_separator(line, '\t');
            if(sep == -1) {
                printf("Something went wrong\n");
                continue;
            }

            if(strncmp(line, lastgram, sep) != 0) {
                unique_count += 1;
                strncpy(lastgram, line, sep);
                lastgram[sep] = '\0';
                if(unique_count % 100000 == 0) {
                    printf("count: %llu\n", unique_count);
                }
            }
        }
    }

    free_paths(&paths);
    return 0;
}

void append_path(struct pathlist *paths, char *filename)
{
    char *fullpath = malloc(strlen(PREFIX) + strlen(filename) + 1);
    strcpy(fullpath, PREFIX);
    strcat(fullpath, filename);

    if(paths->count == paths->capacity) {
        paths->capacity += 10;
        paths->paths = realloc(paths->paths, paths->capacity * sizeof(*(paths->paths)));
    }

    paths->paths[paths->count] = fullpath;
    paths->count += 1;
}

void free_paths(struct pathlist *paths)
{
    int i;
    for(i = 0; i < paths->count; i++) {
        free(paths->paths[i]);
    }

    free(paths->paths);
    paths->count = 0;
    paths->capacity = 0;
}

int find_separator(char *str, char sep)
{
    char *ptr = strchr(str, sep);

    if(ptr == NULL) {
        return -1;
    }

    return (long long int)ptr - (long long int)str;
}
