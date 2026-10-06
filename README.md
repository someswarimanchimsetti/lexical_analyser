# Lexical Analyzer

A C-based Lexical Analyzer that reads a C source file, identifies different types of tokens, and reports lexical errors.

## Features

* Preprocessor directives
* Keywords
* Identifiers
* Integer constants
* Floating-point constants
* Hexadecimal constants
* Character constants
* String literals
* Arithmetic operators
* Assignment operators
* Relational operators
* Logical operators
* Bitwise operators
* Shift operators
* Increment and decrement operators
* Single-line comments
* Multi-line comments
* Special characters
* Bracket matching
* Invalid character detection
* Invalid constant detection
* Unterminated string and comment detection
* Invalid escape sequence detection

## Technologies Used

* C
* GCC
* Make
* Linux / WSL

## Project Structure

```text
lexical_analyser/
├── main.c
├── lexical_analyser.c
├── lexical_analyser.h
├── input.c
├── Makefile
├── .gitignore
└── README.md
```

## Compilation

Build the project using Make:

```bash
make
```

Or compile manually:

```bash
gcc -Wall -Wextra -Werror main.c lexical_analyser.c -o lexical_analyser
```

## Run

```bash
./lexical_analyser
```

The analyzer reads the C source file and displays the identified tokens and detected errors.

## Clean Build Files

To remove generated object files and the executable:

```bash
make clean
```

## Example Input

```c
#include <stdio.h>

int main()
{
    int a = 123;
    float b = 1.2e3;
    char ch = 'A';

    if (a > 10)
    {
        printf("Value is greater");
    }

    return 0;
}
```

## Example Output

```text
#include <stdio.h> : Preprocessor
int : Keyword
main : Identifier
( : Special character
{ : Special character
int : Keyword
a : Identifier
= : Assignment operator
123 : Constant
; : Special character
float : Keyword
b : Identifier
= : Assignment operator
1.2e3 : Constant
; : Special character
char : Keyword
ch : Identifier
= : Assignment operator
'A' : Character
; : Special character
```

## Error Detection

The analyzer detects invalid constructs such as:

```text
12.4.89
1.2y3
```

It also detects:

* Missing or mismatched brackets
* Unterminated comments
* Unterminated strings
* Invalid escape sequences
* Invalid characters
* Invalid numeric constants

## Author

**Someswari Manchimsetti**

B.Tech – Electronics and Communication Engineering

