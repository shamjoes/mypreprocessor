#include <stdio.h>
#include <string.h>
#include "file_inclusion.h"

int main(int argc, char *argv[])
{
    char outname[512];
    char *dot, *slash, *slash1, *slash2;
    FILE *out;

    if (argc != 2) {
        printf("Usage: %s inputfile.c\n", argv[0]);
        return 1;
    }

    /* make the output name: inputfile.c -> inputfile.i */
    strncpy(outname, argv[1], sizeof(outname) - 3);
    outname[sizeof(outname) - 3] = '\0';
    dot = strrchr(outname, '.');
    slash1 = strrchr(outname, '/');
    slash2 = strrchr(outname, '\\');
    slash = slash1 > slash2 ? slash1 : slash2;

    if (dot != NULL && (slash == NULL || dot > slash))
        *dot = '\0';                       /* cut old extension */
    strcat(outname, ".i");

    out = fopen(outname, "w");
    if (out == NULL) {
        printf("Error: cannot create %s\n", outname);
        return 1;
    }

    if (!process_file(argv[1], out, 0)) {
        printf("Error: cannot open %s\n", argv[1]);
        fclose(out);
        remove(outname);
        return 1;
    }

    fclose(out);
    printf("Done. Extended source written to %s\n", outname);
    return 0;
}