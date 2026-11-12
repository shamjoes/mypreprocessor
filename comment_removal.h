#ifndef COMMENT_REMOVAL_H
#define COMMENT_REMOVAL_H

/* Removes both single-line (//) and multi-line (/* ... * /) comments.
   Preserves string literals and newlines for line consistency. */
void remove_comments(const char *src, char *dst);

#endif