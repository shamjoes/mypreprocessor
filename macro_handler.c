#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "macro_handler.h"

#define MAX_MACROS  100
#define MAX_PARAMS  8
#define NAME_LEN    64
#define BODY_LEN    512
#define MAX_PASSES  10

typedef struct {
    char name[NAME_LEN];
    int  is_func;
    int  nparams;
    char params[MAX_PARAMS][NAME_LEN];
    char body[BODY_LEN];
} Macro;

static Macro macros[MAX_MACROS];
static int   macro_count = 0;

static int is_word_start(char c) { return isalpha((unsigned char)c) || c == '_'; }
static int is_word_char(char c)  { return isalnum((unsigned char)c) || c == '_'; }

static void append(char *out, int *j, const char *s)
{
    while (*s != '\0' && *j < MACRO_OUT_LEN - 1)
        out[(*j)++] = *s++;
}

static void append_char(char *out, int *j, char c)
{
    if (*j < MACRO_OUT_LEN - 1)
        out[(*j)++] = c;
}

static int find_macro(const char *name)
{
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0)
            return i;
    }
    return -1;
}

static void trim(char *s)
{
    int len = (int)strlen(s);
    int start = 0;

    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
    while (s[start] != '\0' && isspace((unsigned char)s[start]))
        start++;
    if (start > 0)
        memmove(s, s + start, strlen(s + start) + 1);
}

int is_define_line(const char *line)
{
    const char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '#') return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    return strncmp(p, "define", 6) == 0 && isspace((unsigned char)p[6]);
}

void add_macro(const char *line)
{
    const char *p = line;
    Macro m;
    int k, index;

    memset(&m, 0, sizeof(m));

    while (*p == ' ' || *p == '\t') p++;
    p++; /* # */
    while (*p == ' ' || *p == '\t') p++;
    p += 6; /* define */
    while (*p == ' ' || *p == '\t') p++;

    k = 0;
    while (is_word_char(*p)) {
        if (k < NAME_LEN - 1) m.name[k++] = *p;
        p++;
    }
    m.name[k] = '\0';
    if (k == 0) return;

    if (*p == '(') {
        m.is_func = 1;
        p++;
        while (*p != '\0' && *p != ')') {
            while (*p == ' ' || *p == '\t' || *p == ',') p++;
            k = 0;
            while (is_word_char(*p)) {
                if (k < NAME_LEN - 1 && m.nparams < MAX_PARAMS)
                    m.params[m.nparams][k++] = *p;
                p++;
            }
            if (k > 0)
                m.nparams++;
            else if (*p != '\0' && *p != ')')
                p++;
        }
        if (*p == ')') p++;
    }

    while (*p == ' ' || *p == '\t') p++;
    strncpy(m.body, p, BODY_LEN - 1);
    trim(m.body);

    index = find_macro(m.name);
    if (index >= 0)
        macros[index] = m;
    else if (macro_count < MAX_MACROS)
        macros[macro_count++] = m;
}

static void expand_body(Macro *m, char args[][BODY_LEN], char *out, int *j)
{
    const char *b = m->body;
    char word[NAME_LEN];
    int k, n;

    while (*b != '\0') {
        if (is_word_start(*b)) {
            k = 0;
            while (is_word_char(*b)) {
                if (k < NAME_LEN - 1) word[k++] = *b;
                b++;
            }
            word[k] = '\0';

            for (n = 0; n < m->nparams; n++) {
                if (strcmp(word, m->params[n]) == 0)
                    break;
            }
            if (n < m->nparams)
                append(out, j, args[n]);
            else
                append(out, j, word);
        } else {
            append_char(out, j, *b);
            b++;
        }
    }
}

static int read_args(const char *p, char args[][BODY_LEN], const char **end)
{
    int depth = 0;
    int count = 0;
    int len = 0;

    args[0][0] = '\0';
    for (;;) {
        char c = *p;
        if (c == '\0') return -1;

        if (c == '(') {
            depth++;
            if (depth == 1) { p++; continue; }
        } else if (c == ')') {
            depth--;
            if (depth == 0) {
                args[count][len] = '\0';
                trim(args[count]);
                count++;
                *end = p + 1;
                return count;
            }
        } else if (c == ',' && depth == 1) {
            args[count][len] = '\0';
            trim(args[count]);
            count++;
            if (count >= MAX_PARAMS) return -1;
            len = 0;
            args[count][0] = '\0';
            p++;
            continue;
        }

        if (len < BODY_LEN - 1)
            args[count][len++] = c;
        p++;
    }
}

static int expand_once(const char *in, char *out)
{
    int i = 0, j = 0, changed = 0;
    char word[NAME_LEN];
    char args[MAX_PARAMS][BODY_LEN];

    while (in[i] != '\0') {
        if (in[i] == '"' || in[i] == '\'') {
            char quote = in[i];
            append_char(out, &j, in[i++]);
            while (in[i] != '\0' && in[i] != quote) {
                if (in[i] == '\\' && in[i + 1] != '\0')
                    append_char(out, &j, in[i++]);
                append_char(out, &j, in[i++]);
            }
            if (in[i] != '\0')
                append_char(out, &j, in[i++]);
        }
        else if (isdigit((unsigned char)in[i])) {
            while (is_word_char(in[i]) || in[i] == '.')
                append_char(out, &j, in[i++]);
        }
        else if (is_word_start(in[i])) {
            int k = 0, index, p;
            while (is_word_char(in[i])) {
                if (k < NAME_LEN - 1) word[k++] = in[i];
                i++;
            }
            word[k] = '\0';
            index = find_macro(word);

            if (index < 0) {
                append(out, &j, word);
            }
            else if (!macros[index].is_func) {
                append(out, &j, macros[index].body);
                changed = 1;
            }
            else {
                const char *end = NULL;
                int n;

                p = i;
                while (in[p] == ' ' || in[p] == '\t') p++;

                n = (in[p] == '(') ? read_args(in + p, args, &end) : -1;
                if (n == 1 && args[0][0] == '\0' && macros[index].nparams == 0)
                    n = 0;

                if (n == macros[index].nparams) {
                    expand_body(&macros[index], args, out, &j);
                    i = (int)(end - in);
                    changed = 1;
                } else {
                    append(out, &j, word);
                }
            }
        }
        else {
            append_char(out, &j, in[i++]);
        }
    }
    out[j] = '\0';
    return changed;
}

void apply_macros(const char *in, char *out)
{
    static char temp[MACRO_OUT_LEN];
    int pass;

    strncpy(out, in, MACRO_OUT_LEN - 1);
    out[MACRO_OUT_LEN - 1] = '\0';

    if (macro_count == 0) return;

    for (pass = 0; pass < MAX_PASSES; pass++) {
        int changed = expand_once(out, temp);
        strcpy(out, temp);
        if (!changed) break;
    }
}