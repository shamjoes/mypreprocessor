<div align="center">

# ⚙️ mypreprocessor
### *A Modular C Source Code Preprocessor in C*

[![Language: C99](https://img.shields.io/badge/Language-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![Compiler: GCC](https://img.shields.io/badge/Compiler-GCC-orange.svg)](https://gcc.gnu.org/)
[![Build: Make](https://img.shields.io/badge/Build-Makefile-brightgreen.svg)]()
[![Platform: Linux / POSIX](https://img.shields.io/badge/Platform-Linux%20%7C%20POSIX-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

<p align="center">
  A robust, modular C preprocessor implementing the fundamental lexical analysis and transformation stages of the C translation pipeline: <b>Comment Removal</b>, <b>Header File Inclusion</b>, and <b>Recursive Macro Substitution</b> (both object-like and function-like).
</p>

</div>

---

## 📋 Project Overview & Objectives

In standard C compilation, the preprocessor runs before the compiler proper. It transforms raw `.c` source files into extended translation units (`.i` files) ready for tokenization and parsing.

This tool (`mypreprocessor`) implements:
1. **Comment Removal**:
   - Strips single-line (`//`) and multi-line (`/* ... */`) comments.
   - Retains formatting and line numbers.
   - Preserves string literals (e.g. `printf("// comment in string")` remains untouched).
2. **File Inclusion**:
   - Parses `#include <filename>` and `#include "filename"`.
   - Recursively expands included files up to 10 nested levels to prevent circular loops.
   - Preserves system library headers when external.
3. **Macro Substitution**:
   - Simple object-like macros: `#define PI 3.14`.
   - Function-like macros with arguments: `#define SQUARE(x) ((x)*(x))`.
   - Chained multi-pass macro expansion (e.g., `#define AREA(r) (PI * SQUARE(r))`).

---

## 🏗️ System Architecture & Workflow

```text
       +-----------------------+
       |   Input: inputfile.c  |
       +-----------+-----------+
                   |
                   v
       +-----------------------+
       | 1. Comment Removal    |  --> Strips // and /* ... */
       |    (comment_removal.c)|      (protects quotes & escapes)
       +-----------+-----------+
                   |
                   v
       +-----------------------+
       | 2. File Inclusion     |  --> Recursively inlines #include
       |    (file_inclusion.c) |      (resolves local & relative paths)
       +-----------+-----------+
                   |
                   v
       +-----------------------+
       | 3. Macro Substitution |  --> Stores #define table
       |    (macro_handler.c)  |      Evaluates arguments & expands
       +-----------+-----------+
                   |
                   v
       +-----------------------+
       |  Extended Source File |
       |     inputfile.i       |
       +-----------------------+
```

---

## 📂 Modular Structure

| Module | Source / Header | Responsibility |
|---|---|---|
| **Driver Engine** | `main.c` | CLI argument validation, path processing, `.i` stream creation. |
| **Comment Filter** | `comment_removal.c` / `.h` | In-memory token scanner removing single and multi-line comments. |
| **File Includer** | `file_inclusion.c` / `.h` | Reads source files, discovers directives, recursively inlines headers. |
| **Macro Processor** | `macro_handler.c` / `.h` | Macro symbol table, argument tokenization, recursive body expansion. |
| **Build Script** | `Makefile` | Automated compilation targets (`all`, `test`, `clean`). |
| **Verification** | `test.c`, `myheader.h` | Sample test suite exercising all preprocessor rules. |

---

## 🚀 Build & Execution Guide

### Prerequisites
* GCC (GNU Compiler Collection) or Clang
* GNU Make
* Linux / POSIX terminal (or WSL on Windows)

### 1. Build the Executable
```bash
make
```
This produces the executable `mypreprocessor`.

### 2. Execute on C Source File
```bash
./mypreprocessor test.c
```
The program will display:
```text
Done. Extended source written to test.i
```

### 3. Run Automated Test Target
```bash
make test
```

### 4. Clean Generated Artifacts
```bash
make clean
```

---

## 🧪 Verification & Expected Output

### Input Source (`test.c`)
```c
// Test program for mypreprocessor
#include <stdio.h>
#include "myheader.h"

#define PI 3.14
#define SQUARE(x) ((x)*(x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define AREA(r) (PI * SQUARE(r))

/* multi line
   comment here */
int main()
{
    int arr[MAX_SIZE];            // array declaration
    float a = AREA(5);            /* macro chained expansion */
    int m = MAX(SQUARE(2), 7);
    printf("PI is // not a comment, PI\n");
    return 0;
}
```

### Generated Extended Output (`test.i`)
```c
#include <stdio.h>

int add(int a, int b);


int main()
{
    int arr[100];            
    float a = (3.14 * ((5)*(5)));            
    int m = (((2)*(2)) > (7) ? (((2)*(2))) : (7));
    printf("PI is // not a comment, PI\n");
    return 0;
}
```

---

## 🎓 Viva & Reviewer Preparation Guide

### Q1: Why must comments inside double quotes not be removed?
**A:** In C, sequences like `//` or `/*` inside string literals (e.g. `printf("http://...")`) are part of the string payload. The preprocessor maintains a quotation state tracker; when inside quotes, characters are preserved verbatim.

### Q2: How does the tool prevent infinite recursion in file inclusion?
**A:** `process_file` passes a `depth` parameter. If recursive inclusion exceeds `MAX_DEPTH` (default 10), inclusion halts with an error, preventing circular header traps (`a.h` includes `b.h` which includes `a.h`).

### Q3: How are parameterized macros expanded?
**A:** When a macro call `SQUARE(x)` is detected, `read_args` parses comma-separated arguments while respecting parenthesis depth. `expand_body` then scans the macro body token-by-token and substitutes formal parameters with the evaluated actual arguments.

---

## 📜 License
Distributed under the **MIT License**. See `LICENSE` for details.