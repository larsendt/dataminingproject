#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <zlib.h>
#include <regex.h>

#define PREFIX "/mnt/storage/data_mining_project/data/4grams-fiction-tmp/"
#define OUTFMT "/mnt/storage/data_mining_project/data/4grams-fiction-pruned/pruned-%s"
#define LINESIZE 4096
#define PATHSIZE 4096
#define REGEXSTR "(.*_PRON.*|.*_NOUN.*|.*_ADJ.*|.*_ADV.*|.*_NUM.*|.*_DET.*|.*_CONJ.*|.*_END.*|.*_ADP.*|.*_X.*|.*_PRT.*|.*_START.*|.*_VERB.*|.*_\\.|.* \\. .*|.*,.*|.*\".*|.*\\(.*|.*\\).*|.*\\[.*|.*\\].*|.* \\..*|.*\\. .*|.*\\-\\-.*|.*\\-.*|.* \\: .*|.*\\? .*|.* \\? .*|.*\\{.*|.*\\}.*|.* ' .*|.* '.*)"

struct pathlist {
    char **paths;
    char **filenames;
    int count;
    int capacity;
};

void append_path(struct pathlist *paths, char *filename);
void free_paths(struct pathlist *paths);
int find_separator(char *str, char sep);
int find_nth_separator(char *str, char sep, int n);

int main(int argc, char *argv[])
{
    struct pathlist paths;
    paths.count = 0;
    paths.capacity = 0;
    paths.paths = NULL;
    paths.filenames = NULL;

    gzFile gz_in;
    gzFile gz_out;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(PREFIX)) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            append_path(&paths, ent->d_name);
        }
        closedir(dir);
    }
    else {
        perror(PREFIX);
        return 1;
    }

    char line[LINESIZE];
    char outpath[PATHSIZE];
    char *ptr;
    int year;
    int total_count;
    int book_count;
    int ret;
    int sep;
    char *tmpline;
    long long int read_count = 0;
    long long int write_count = 0;
    regex_t regex;
    int reti = regcomp(&regex, REGEXSTR, REG_EXTENDED);
    if(reti) {
        printf("bad regex\n");
        exit(1);
    }

    printf("%d files to process\n", paths.count);
    for(int i = 0; i < paths.count; i++) {
        gz_in = gzopen(paths.paths[i], "rb");
        sprintf(outpath, OUTFMT, paths.filenames[i]);
        gz_out = gzopen(outpath, "wb");
        while((ptr = gzgets(gz_in, line, LINESIZE)) != NULL) {
            read_count += 1;
            sep = find_separator(line, '\t');
            tmpline = line + sep + 1;
            ret = sscanf(tmpline, "%d\t%d\t%d", &year, &total_count, &book_count);
            if(ret == 3) {
                reti = regexec(&regex, line, 0, NULL, 0);
                if((year >= 1900) && reti) {
                    gzputs(gz_out, line);
                    write_count += 1;
                }
            }
            else {
                printf("sscanf only scanned %d items\n", ret);
                continue;
            }

            if(read_count % 1000000 == 0) {
                printf("read: %lld lines, write: %lld lines, frac: %.2f\n", read_count, write_count, (float)write_count/read_count);
            }
        }
        gzclose(gz_in);
        gzclose(gz_out);
    }

    free_paths(&paths);
    return 0;
}

void append_path(struct pathlist *paths, char *filename)
{
    char *fullpath = malloc(strlen(PREFIX) + strlen(filename) + 1);
    strcpy(fullpath, PREFIX);
    strcat(fullpath, filename);

    char *fnamecopy = malloc(strlen(filename) + 1);
    strcpy(fnamecopy, filename);

    if(paths->count == paths->capacity) {
        paths->capacity += 10;
        paths->paths = realloc(paths->paths, paths->capacity * sizeof(*(paths->paths)));
        paths->filenames = realloc(paths->filenames, paths->capacity * sizeof(*(paths->filenames)));
    }

    paths->paths[paths->count] = fullpath;
    paths->filenames[paths->count] = fnamecopy;
    paths->count += 1;
}

void free_paths(struct pathlist *paths)
{
    for(int i = 0; i < paths->count; i++) {
        free(paths->paths[i]);
        free(paths->filenames[i]);
    }

    free(paths->paths);
    free(paths->filenames);
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

int find_nth_separator(char *str, char sep, int n)
{
    char *ptr = str;
    for(int i = 0; i < n; i++) {
        ptr = strchr(ptr, sep);
        if(ptr == NULL) {
            return -1;
        }
    }

    return (long long int)ptr - (long long int)str;
}
