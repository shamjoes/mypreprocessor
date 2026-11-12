#ifndef MACRO_HANDLER_H
#define MACRO_HANDLER_H

#define MACRO_OUT_LEN 4096

int is_define_line(const char *line);
void add_macro(const char *line);
void apply_macros(const char *in, char *out);

#endif