#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <zlib.h>
#include <regex.h>

#include "hashtable.h"

#define PREFIX "/mnt/storage/data_mining_project/data/5grams-pruned/"
#define OUTFILE "/home/dane/Desktop/top_100_5grams.txt"
#define METAFILE "/mnt/storage/data_mining_project/data/5grams-info/eng-us-all-totalcounts.txt"
#define LINESIZE 4096
#define PATHSIZE 4096

struct pathlist {
    char **paths;
    char **filenames;
    int count;
    int capacity;
};

typedef struct node {
    char *gram;
    double three_year_rate;
    int year;
    struct node *prev;
    struct node *next;
} node;

typedef struct linked_list {
    node *head;
    node *tail;
    int count;
    double max;
    double min;
} linked_list;

node *make_node(char *gram, double rate, int year);
void free_node(node *n);

void ll_insert(linked_list *l, node *n);
void ll_destroy(linked_list *l);

node *make_node(char *gram, double rate, int year)
{
    char *gramcopy = malloc(strlen(gram) + 1);
    strcpy(gramcopy, gram);
    node *n = malloc(sizeof(node));
    n->gram = gramcopy;
    n->three_year_rate = rate;
    n->year = year;
    n->prev = NULL;
    n->next = NULL;
    return n;
}


void free_node(node *n)
{
    free(n->gram);
    free(n);
}

double abs_val(double val) {
    return val > 0 ? val : -val;
}

int test(linked_list *l) {
    int c = 0;
    node *n = l->head;
    while(n != NULL) {
        c += 1;
        n = n->next;

        if(c > l->count) {
            return 1;
        }
    }
    return 0;
}

void ll_insert(linked_list *l, node *n)
{
    node *cur = l->head;
    if(cur == NULL) {
        l->head = n;
        l->tail = n;
        l->count = 1;
        l->max = abs_val(n->three_year_rate);
        l->min = abs_val(n->three_year_rate);
        return;
    }

    while(1) {
        if(abs_val(n->three_year_rate) >= abs_val(cur->three_year_rate)) {
            n->prev = cur->prev;
            n->next = cur;
            if(cur->prev != NULL) {
                cur->prev->next = n;
            }
            cur->prev = n;
            l->count += 1;

            if(cur == l->head) {
                l->head = n;
                l->max = abs_val(n->three_year_rate);
                n->prev = NULL;
            }
            break;
        }
        else {
            if(cur->next == NULL) {
                cur->next = n;
                n->prev = cur;
                n->next = NULL;
                l->min = abs_val(n->three_year_rate);
                l->count += 1;
                l->tail = n;
                break;
            }
            else {
                cur = cur->next;
            }
        }
    }

    if(l->count > 100) {
        node *byebye = l->tail;
        l->tail = byebye->prev;
        l->min = abs_val(l->tail->three_year_rate);
        l->tail->next = NULL;
        l->count -= 1;
        free_node(byebye);
        printf("min: %s %d %e\nmax: %s %d %e\n", l->tail->gram, l->tail->year, l->tail->three_year_rate, l->head->gram, l->head->year, l->head->three_year_rate);
    }
}

void ll_destroy(linked_list *l)
{
    node *cur = l->head;
    node *tmp;
    while(cur != NULL) {
        tmp = cur->next;
        free_node(cur);
        cur = tmp;
    }
}

void append_path(struct pathlist *paths, char *filename);
void free_paths(struct pathlist *paths);
int find_separator(char *str, char sep);
int find_nth_separator(char *str, char sep, int n);
int unique_mentions(char *str);
int year_for_gram(char *str);
void get_ngram(char *str, char *gram);
hash_table *normalization_hashtable();

int main(int argc, char *argv[])
{
    struct pathlist paths;
    paths.count = 0;
    paths.capacity = 0;
    paths.paths = NULL;
    paths.filenames = NULL;

    gzFile gz_in;

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(PREFIX)) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if(ent->d_name[0] != '.') {
                append_path(&paths, ent->d_name);
            }
        }
        closedir(dir);
    }
    else {
        perror(PREFIX);
        return 1;
    }

    char line[LINESIZE];
    char current_ngram[LINESIZE];
    char new_ngram[LINESIZE];
    char outpath[PATHSIZE];
    double rates_of_change[200];
    int smoothed_years_list[200];
    int years_list[300];
    double uniques_list[300];
    char *ptr;
    char *tmpline;
    long long int read_count = 0;
    long long int write_count = 0;
    int unique_counter = 0;
    int rate_counter = 0;

    linked_list ll;
    ll.count = 0;
    ll.head = NULL;
    ll.tail = NULL;
    ll.max = 0;
    ll.min = 0;

    hash_table *ht = normalization_hashtable();

    printf("%d files to process\n", paths.count);
    for(int i = 0; i < paths.count; i++) {
        gz_in = gzopen(paths.paths[i], "rb");
        printf("gzfile: %s\n", paths.paths[i]);
        while((ptr = gzgets(gz_in, line, LINESIZE)) != NULL) {
            read_count += 1;

            get_ngram(line, new_ngram);
            if(!strcmp(new_ngram, current_ngram)) {
                int uniques = unique_mentions(line);
                int year = year_for_gram(line);

                long long unsigned int key = year;
                long long unsigned int *value;
                size_t value_size;
                if(!ht_contains(ht, &key, sizeof(key))) {
                    printf("skipped year: %llu\n", key);
                    continue;
                }
                value = ht_get(ht, &key, sizeof(key), &value_size);

                double normed_count = (double)uniques/(*value);

                uniques_list[unique_counter] = normed_count;
                years_list[unique_counter] = year;
                unique_counter += 1;
            }
            else {
                rate_counter = 0;
                for(int j = 0; j < unique_counter; j+=3) {
                    double d1 = uniques_list[j+1] - uniques_list[j];
                    double d2 = uniques_list[j+2] - uniques_list[j+1];
                    rates_of_change[rate_counter] = (d1 + d2) / 2.0;
                    smoothed_years_list[rate_counter] = years_list[j];
                    rate_counter += 1;
                }

                for(int j = 0; j < rate_counter; j++) {
                    if(ll.count < 100 || abs_val(rates_of_change[j]) > ll.min) {
                        node *n = make_node(current_ngram, rates_of_change[j], smoothed_years_list[j]);
                        ll_insert(&ll, n);
                    }
                }

                strcpy(current_ngram, new_ngram);
                unique_counter = 0;
            }


            if(read_count % 1000000 == 0) {
                printf("read: %lld lines\n", read_count);
            }
        }
        gzclose(gz_in);
    }

    FILE *outf = fopen(OUTFILE, "w");
    node *n = ll.head;

    while(n != NULL) {
        fprintf(outf, "%s %d %e\n", n->gram, n->year, n->three_year_rate);
        n = n->next;
    }

    fclose(outf);
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

int unique_mentions(char *str)
{
    char *ptr = strchr(str, '\t');
    if(ptr == NULL) {
        return -1;
    }

    ptr += 1;

    int year, total, unique;
    int ret = sscanf(ptr, "%d\t%d\t%d", &year, &total, &unique);
    if(ret != 3) {
        fprintf(stderr, "Unique mentions failed to parse data\n");
    }
    return unique;
}

int year_for_gram(char *str)
{
    char *ptr = strchr(str, '\t');
    if(ptr == NULL) {
        return -1;
    }

    ptr += 1;

    int year, total, unique;
    int ret = sscanf(ptr, "%d\t%d\t%d", &year, &total, &unique);
    if(ret != 3) {
        fprintf(stderr, "Year for gram failed to parse data\n");
    }
    return year;
}

void get_ngram(char *str, char *gram)
{
    int sep = find_separator(str, '\t');
    strncpy(gram, str, sep);
    gram[sep] = '\0';
}

hash_table *normalization_hashtable()
{
    FILE *f = fopen(METAFILE, "r");
    fseek(f, 0, SEEK_END);
    int sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(sz+1);
    fread(buf, 1, sz, f);
    fclose(f);
    buf[sz] = '\0';

    hash_table *ht = malloc(sizeof(hash_table));
    ht_init(ht, 0, 0.2);
    long long unsigned int year;
    long long unsigned int total;
    int ret;
    char *ptr = buf;
    while(ptr != NULL) {
        ret = sscanf(ptr, "%llu,%llu", &year, &total);
        if(ret == 2) {
            ht_insert(ht, &year, sizeof(year), &total, sizeof(total));
        }
        else {
            printf("norm ht derped (%d) (%s)\n", ret, ptr);
        }

        ptr = strchr(ptr, '\t');
        if(ptr != NULL) {
            ptr += 1;
        }
    }

    return ht;
}
