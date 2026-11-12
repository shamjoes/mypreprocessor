#ifndef FILE_INCLUSION_H
#define FILE_INCLUSION_H

#include <stdio.h>

char *read_file(const char *path);
int process_file(const char *path, FILE *out, int depth);

#endif