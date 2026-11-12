#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_inclusion.h"
#include "comment_removal.h"
#include "macro_handler.h"

#define LINE_LEN   2048
#define MAX_DEPTH  10      /* protects against circular file inclusions */

char *read_file(const char *path)
{
    FILE *fp = fopen(path, "rb");
    long size;
    char *buffer;

    if (fp == NULL)
        return NULL;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    buffer = malloc(size + 1);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }
    size = fread(buffer, 1, size, fp);
    buffer[size] = '\0';

    fclose(fp);
    return buffer;
}

static int get_include_name(const char *line, char *name)
{
    const char *p = line;
    char closing;
    int k = 0;

    while (*p == ' ' || *p == '\t') p++;
    if (*p != '#') return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (strncmp(p, "include", 7) != 0) return 0;
    p += 7;
    while (*p == ' ' || *p == '\t') p++;

    if (*p == '<')       closing = '>';
    else if (*p == '"')  closing = '"';
    else                 return 0;
    p++;

    while (*p != '\0' && *p != closing && k < 255)
        name[k++] = *p++;
    name[k] = '\0';
    return k > 0;
}

int process_file(const char *path, FILE *out, int depth)
{
    char *source = read_file(path);
    char *clean;
    const char *p;
    char line[LINE_LEN];
    char result[MACRO_OUT_LEN];
    char name[256];

    if (source == NULL)
        return 0;

    /* step 1: remove comments */
    clean = malloc(strlen(source) + 1);
    if (clean == NULL) {
        free(source);
        return 0;
    }
    remove_comments(source, clean);
    free(source);

    /* step 2: process lines */
    p = clean;
    while (*p != '\0') {
        int full_len, copy_len;
        const char *newline = strchr(p, '\n');

        full_len = newline ? (int)(newline - p) : (int)strlen(p);
        copy_len = full_len < LINE_LEN - 1 ? full_len : LINE_LEN - 1;
        memcpy(line, p, copy_len);
        line[copy_len] = '\0';
        p += full_len;
        if (*p == '\n') p++;

        if (get_include_name(line, name)) {
            int ok = 0;
            if (depth < MAX_DEPTH) {
                ok = process_file(name, out, depth + 1);

                /* try relative directory of current file */
                if (!ok) {
                    char *slash1 = strrchr(path, '/');
                    char *slash2 = strrchr(path, '\\');
                    char *last_slash = slash1 > slash2 ? slash1 : slash2;
                    if (last_slash != NULL) {
                        char full[512];
                        int dir_len = (int)(last_slash - path + 1);
                        snprintf(full, sizeof(full), "%.*s%s", dir_len, path, name);
                        ok = process_file(full, out, depth + 1);
                    }
                }
            }
            if (!ok) {
                /* Preserve system include line */
                fprintf(stderr, "Note: keeping external include '%s'\n", name);
                fprintf(out, "%s\n", line);
            }
        }
        else if (is_define_line(line)) {
            /* store macro, omit directive from extended source */
            add_macro(line);
        }
        else {
            /* replace macros and emit */
            apply_macros(line, result);
            fprintf(out, "%s\n", result);
        }
    }

    free(clean);
    return 1;
}