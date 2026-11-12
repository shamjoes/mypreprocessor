#include "comment_removal.h"

void remove_comments(const char *src, char *dst)
{
    int i = 0;   /* position in src */
    int j = 0;   /* position in dst */

    while (src[i] != '\0') {
        /* 1. String or character literal: preserve as-is */
        if (src[i] == '"' || src[i] == '\'') {
            char quote = src[i];
            dst[j++] = src[i++];
            while (src[i] != '\0' && src[i] != quote) {
                if (src[i] == '\\' && src[i + 1] != '\0')
                    dst[j++] = src[i++];
                dst[j++] = src[i++];
            }
            if (src[i] != '\0')
                dst[j++] = src[i++];
        }
        /* 2. Single line comment: skip until newline */
        else if (src[i] == '/' && src[i + 1] == '/') {
            while (src[i] != '\0' && src[i] != '\n')
                i++;
        }
        /* 3. Multi line comment: skip until closing star-slash */
        else if (src[i] == '/' && src[i + 1] == '*') {
            i += 2;
            while (src[i] != '\0' && !(src[i] == '*' && src[i + 1] == '/')) {
                if (src[i] == '\n')
                    dst[j++] = '\n';
                i++;
            }
            if (src[i] != '\0')
                i += 2;
            dst[j++] = ' ';   /* multi-line comment acts as a space */
        }
        /* 4. Regular characters */
        else {
            dst[j++] = src[i++];
        }
    }
    dst[j] = '\0';
}