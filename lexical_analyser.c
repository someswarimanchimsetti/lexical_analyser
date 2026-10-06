#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexical_analyser.h"

#define MAX_STACK 100

int is_keyword(const char *word)
{
    const char *keywords[] =
        {
            "auto", "break", "case", "char", "const", "continue",
            "default", "do", "double", "else", "enum", "extern",
            "float", "for", "goto", "if", "int", "long",
            "register", "return", "short", "signed", "sizeof",
            "static", "struct", "switch", "typedef", "union",
            "unsigned", "void", "volatile", "while"};

    int size = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < size; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int is_valid_escape(int ch)
{
    if (ch == 'n' || ch == 't' || ch == 'r' ||
        ch == 'b' || ch == 'f' || ch == 'v' ||
        ch == 'a' || ch == '\\' || ch == '\'' ||
        ch == '"' || ch == '0')
    {
        return 1;
    }

    return 0;
}

void analyse_file(const char *filename)
{
    FILE *fp;
    char word[100];

    int ch;
    int next;
    int line = 1;

    char stack[MAX_STACK];
    int stack_line[MAX_STACK];
    int top = -1;

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        /* New line */
        if (ch == '\n')
        {
            line++;
            continue;
        }

        /* White space */
        if (isspace(ch))
        {
            continue;
        }

        /* Preprocessor */
        if (ch == '#')
        {
            int i = 0;

            word[i++] = ch;

            while ((ch = fgetc(fp)) != EOF && ch != '\n')
            {
                if (i < 99)
                {
                    word[i++] = ch;
                }
            }

            word[i] = '\0';

            printf("%s : Preprocessor\n", word);

            if (ch == '\n')
            {
                line++;
            }

            continue;
        }

        /* Identifier / Keyword */
        if (isalpha(ch) || ch == '_')
        {
            int i = 0;

            word[i++] = ch;

            while ((ch = fgetc(fp)) != EOF &&
                   (isalnum(ch) || ch == '_'))
            {
                if (i < 99)
                {
                    word[i++] = ch;
                }
            }

            word[i] = '\0';

            if (is_keyword(word))
            {
                printf("%s : Keyword\n", word);
            }
            else
            {
                printf("%s : Identifier\n", word);
            }

            if (ch != EOF)
            {
                ungetc(ch, fp);
            }

            continue;
        }

        /* Numbers */
        if (isdigit(ch) || ch == '.')
        {
            int i = 0;
            int invalid = 0;
            int exponent_error = 0;

            /* Number beginning with '.' */
            if (ch == '.')
            {
                next = fgetc(fp);

                if (isdigit(next))
                {
                    word[i++] = ch;
                    word[i++] = next;

                    while ((ch = fgetc(fp)) != EOF && isdigit(ch))
                    {
                        if (i < 99)
                        {
                            word[i++] = ch;
                        }
                    }

                    if (ch == '.')
                    {
                        invalid = 1;

                        if (i < 99)
                        {
                            word[i++] = ch;
                        }

                        while ((ch = fgetc(fp)) != EOF &&
                               (isdigit(ch) || ch == '.'))
                        {
                            if (i < 99)
                            {
                                word[i++] = ch;
                            }
                        }
                    }

                    word[i] = '\0';

                    if (invalid)
                    {
                        printf("Line %d: Error: Invalid constant: %s\n",
                               line, word);
                    }
                    else
                    {
                        printf("%s : Constant\n", word);
                    }

                    if (ch != EOF)
                    {
                        ungetc(ch, fp);
                    }

                    continue;
                }

                printf(". : Special character\n");

                if (next != EOF)
                {
                    ungetc(next, fp);
                }

                continue;
            }

            /* Hexadecimal constant */
            if (ch == '0')
            {
                next = fgetc(fp);

                if (next == 'x' || next == 'X')
                {
                    word[i++] = ch;
                    word[i++] = next;

                    /* First hexadecimal digit */
                    next = fgetc(fp);

                    if (!isxdigit(next))
                    {
                        word[i] = '\0';

                        printf("Line %d: Error: Invalid constant: %s\n",
                               line, word);

                        if (next != EOF)
                        {
                            ungetc(next, fp);
                        }

                        continue;
                    }

                    word[i++] = next;

                    /* Remaining hexadecimal digits */
                    while ((ch = fgetc(fp)) != EOF &&
                           isxdigit(ch))
                    {
                        if (i < 99)
                        {
                            word[i++] = ch;
                        }
                    }

                    /* Hexadecimal followed by identifier */
                    if (isalpha(ch) || ch == '_')
                    {
                        invalid = 1;

                        if (i < 99)
                        {
                            word[i++] = ch;
                        }

                        while ((ch = fgetc(fp)) != EOF &&
                               (isalnum(ch) || ch == '_'))
                        {
                            if (i < 99)
                            {
                                word[i++] = ch;
                            }
                        }
                    }

                    word[i] = '\0';

                    if (invalid)
                    {
                        printf("Line %d: Error: Invalid constant: %s\n",
                               line, word);
                    }
                    else
                    {
                        printf("%s : Constant\n", word);
                    }

                    if (ch != EOF)
                    {
                        ungetc(ch, fp);
                    }

                    continue;
                }

                if (next != EOF)
                {
                    ungetc(next, fp);
                }
            }

            /* Decimal / Floating constant */

            word[i++] = ch;

            /* Integer part */
            while ((ch = fgetc(fp)) != EOF && isdigit(ch))
            {
                if (i < 99)
                {
                    word[i++] = ch;
                }
            }

            /* Decimal point */
            if (ch == '.')
            {
                word[i++] = ch;

                next = fgetc(fp);

                /* Digit required after decimal point */
                if (!isdigit(next))
                {
                    word[i] = '\0';

                    printf("Line %d: Error: Invalid constant: %s\n",
                           line, word);

                    if (next != EOF)
                    {
                        ungetc(next, fp);
                    }

                    continue;
                }

                /* First digit after '.' */
                word[i++] = next;

                while ((ch = fgetc(fp)) != EOF && isdigit(ch))
                {
                    if (i < 99)
                    {
                        word[i++] = ch;
                    }
                }

                /* Second decimal point */
                if (ch == '.')
                {
                    invalid = 1;

                    if (i < 99)
                    {
                        word[i++] = ch;
                    }

                    while ((ch = fgetc(fp)) != EOF &&
                           (isdigit(ch) || ch == '.'))
                    {
                        if (i < 99)
                        {
                            word[i++] = ch;
                        }
                    }
                }
            }

            /* Exponent */
            if (!invalid && (ch == 'e' || ch == 'E'))
            {
                if (i < 99)
                {
                    word[i++] = ch;
                }

                next = fgetc(fp);

                /* Optional + or - */
                if (next == '+' || next == '-')
                {
                    if (i < 99)
                    {
                        word[i++] = next;
                    }

                    next = fgetc(fp);
                }

                /* Exponent must contain at least one digit */
                if (!isdigit(next))
                {
                    invalid = 1;
                    exponent_error = 1;

                    /*
                     * Do not consume the character after 'e'.
                     * It belongs to the next token.
                     */
                    if (next != EOF)
                    {
                        ungetc(next, fp);
                    }

                    ch = next;
                }
                else
                {
                    /* First exponent digit */
                    word[i++] = next;

                    while ((ch = fgetc(fp)) != EOF && isdigit(ch))
                    {
                        if (i < 99)
                        {
                            word[i++] = ch;
                        }
                    }
                }
            }

            /*
             * Number followed by identifier.
             *
             * Do not execute this check when the exponent itself
             * was invalid, because 'e' already belongs to the
             * invalid constant.
             */
            if (!exponent_error && (isalpha(ch) || ch == '_'))
            {
                invalid = 1;

                if (i < 99)
                {
                    word[i++] = ch;
                }

                while ((ch = fgetc(fp)) != EOF &&
                       (isalnum(ch) || ch == '_'))
                {
                    if (i < 99)
                    {
                        word[i++] = ch;
                    }
                }
            }

            word[i] = '\0';

            if (invalid)
            {
                printf("Line %d: Error: Invalid constant: %s\n",
                       line, word);
            }
            else
            {
                printf("%s : Constant\n", word);
            }

            if (ch != EOF)
            {
                ungetc(ch, fp);
            }

            continue;
        }

        /* Comments / Division */
        if (ch == '/')
        {
            next = fgetc(fp);

            /* Single-line comment */
            if (next == '/')
            {
                int i = 0;

                word[i++] = '/';
                word[i++] = '/';

                while ((ch = fgetc(fp)) != EOF && ch != '\n')
                {
                    if (i < 99)
                    {
                        word[i++] = ch;
                    }
                }

                word[i] = '\0';

                printf("%s : Comment\n", word);

                if (ch == '\n')
                {
                    line++;
                }

                continue;
            }

            /* Multi-line comment */
            if (next == '*')
            {
                int i = 0;
                int previous = 0;
                int closed = 0;

                word[i++] = '/';
                word[i++] = '*';

                while ((ch = fgetc(fp)) != EOF)
                {
                    if (i < 99)
                    {
                        word[i++] = ch;
                    }

                    if (ch == '\n')
                    {
                        line++;
                    }

                    if (previous == '*' && ch == '/')
                    {
                        closed = 1;
                        break;
                    }

                    previous = ch;
                }

                word[i] = '\0';

                if (!closed)
                {
                    printf("Line %d: Error: Unterminated comment\n",
                           line);
                }
                else
                {
                    printf("%s : Comment\n", word);
                }

                continue;
            }

            /* Division assignment */
            if (next == '=')
            {
                printf("/= : Operator\n");
            }
            else
            {
                printf("/ : Arithmetic operator\n");

                if (next != EOF)
                {
                    ungetc(next, fp);
                }
            }

            continue;
        }

        /* String */
        if (ch == '"')
        {
            int i = 0;
            int closed = 0;
            int valid = 1;

            word[i++] = ch;

            while ((ch = fgetc(fp)) != EOF)
            {
                if (i < 99)
                {
                    word[i++] = ch;
                }

                if (ch == '\\')
                {
                    next = fgetc(fp);

                    if (next == EOF)
                    {
                        break;
                    }

                    if (i < 99)
                    {
                        word[i++] = next;
                    }

                    if (!is_valid_escape(next))
                    {
                        valid = 0;
                    }

                    continue;
                }

                if (ch == '"')
                {
                    closed = 1;
                    break;
                }

                if (ch == '\n')
                {
                    line++;
                    break;
                }
            }

            word[i] = '\0';

            if (!closed)
            {
                printf("Line %d: Error: Unterminated string: %s\n",
                       line, word);
            }
            else if (!valid)
            {
                printf("Line %d: Error: Invalid escape sequence: %s\n",
                       line, word);
            }
            else
            {
                printf("%s : String\n", word);
            }

            continue;
        }

        /* Character constant */
        if (ch == '\'')
        {
            int i = 0;
            int closed = 0;
            int valid = 1;
            int char_count = 0;

            word[i++] = ch;

            while ((ch = fgetc(fp)) != EOF)
            {
                if (i < 99)
                {
                    word[i++] = ch;
                }

                if (ch == '\\')
                {
                    next = fgetc(fp);

                    if (next == EOF)
                    {
                        break;
                    }

                    if (i < 99)
                    {
                        word[i++] = next;
                    }

                    if (!is_valid_escape(next))
                    {
                        valid = 0;
                    }

                    char_count++;
                    continue;
                }

                if (ch == '\'')
                {
                    closed = 1;
                    break;
                }

                if (ch == '\n')
                {
                    line++;
                    break;
                }

                char_count++;
            }

            word[i] = '\0';

            if (!closed)
            {
                printf("Line %d: Error: Unterminated character constant: %s\n",
                       line, word);
            }
            else if (!valid || char_count != 1)
            {
                printf("Line %d: Error: Invalid character constant: %s\n",
                       line, word);
            }
            else
            {
                printf("%s : Character\n", word);
            }

            continue;
        }

        /* Two-character operators */
        if (ch == '+' || ch == '-' ||
            ch == '=' || ch == '!' ||
            ch == '<' || ch == '>' ||
            ch == '&' || ch == '|' ||
            ch == '*' || ch == '%')
        {
            next = fgetc(fp);

            if (ch == '+' && next == '+')
            {
                printf("++ : Operator\n");
                continue;
            }

            if (ch == '+' && next == '=')
            {
                printf("+= : Operator\n");
                continue;
            }

            if (ch == '-' && next == '-')
            {
                printf("-- : Operator\n");
                continue;
            }

            if (ch == '-' && next == '=')
            {
                printf("-= : Operator\n");
                continue;
            }

            if (ch == '=' && next == '=')
            {
                printf("== : Relational operator\n");
                continue;
            }

            if (ch == '!' && next == '=')
            {
                printf("!= : Relational operator\n");
                continue;
            }

            if (ch == '<' && next == '=')
            {
                printf("<= : Relational operator\n");
                continue;
            }

            if (ch == '>' && next == '=')
            {
                printf(">= : Relational operator\n");
                continue;
            }

            if (ch == '<' && next == '<')
            {
                printf("<< : Operator\n");
                continue;
            }

            if (ch == '>' && next == '>')
            {
                printf(">> : Operator\n");
                continue;
            }

            if (ch == '&' && next == '&')
            {
                printf("&& : Logical operator\n");
                continue;
            }

            if (ch == '|' && next == '|')
            {
                printf("|| : Logical operator\n");
                continue;
            }

            if (ch == '*' && next == '=')
            {
                printf("*= : Operator\n");
                continue;
            }

            if (ch == '%' && next == '=')
            {
                printf("%%= : Operator\n");
                continue;
            }

            if (next != EOF)
            {
                ungetc(next, fp);
            }

            if (ch == '+')
            {
                printf("+ : Arithmetic operator\n");
            }
            else if (ch == '-')
            {
                printf("- : Arithmetic operator\n");
            }
            else if (ch == '=')
            {
                printf("= : Assignment operator\n");
            }
            else if (ch == '!')
            {
                printf("! : Logical operator\n");
            }
            else if (ch == '<')
            {
                printf("< : Relational operator\n");
            }
            else if (ch == '>')
            {
                printf("> : Relational operator\n");
            }
            else if (ch == '&')
            {
                printf("& : Operator\n");
            }
            else if (ch == '|')
            {
                printf("| : Operator\n");
            }
            else if (ch == '*')
            {
                printf("* : Arithmetic operator\n");
            }
            else if (ch == '%')
            {
                printf("%% : Arithmetic operator\n");
            }

            continue;
        }

        /* Bitwise NOT / XOR */
        if (ch == '~' || ch == '^')
        {
            printf("%c : Operator\n", ch);
            continue;
        }

        /* Ternary operators */
        if (ch == '?' || ch == ':')
        {
            printf("%c : Special character\n", ch);
            continue;
        }

        /* Opening brackets */
        if (ch == '(' || ch == '{' || ch == '[')
        {
            printf("%c : Special character\n", ch);

            if (top < MAX_STACK - 1)
            {
                top++;
                stack[top] = ch;
                stack_line[top] = line;
            }

            continue;
        }

        /* Closing brackets */
        if (ch == ')' || ch == '}' || ch == ']')
        {
            printf("%c : Special character\n", ch);

            if (top == -1)
            {
                if (ch == ')')
                {
                    printf("Line %d: Error: Unexpected ')'\n", line);
                }
                else if (ch == '}')
                {
                    printf("Line %d: Error: Unexpected '}'\n", line);
                }
                else
                {
                    printf("Line %d: Error: Unexpected ']'\n", line);
                }
            }
            else
            {
                char expected;

                if (stack[top] == '(')
                {
                    expected = ')';
                }
                else if (stack[top] == '{')
                {
                    expected = '}';
                }
                else
                {
                    expected = ']';
                }

                if (ch == expected)
                {
                    top--;
                }
                else
                {
                    printf("Line %d: Error: Mismatched bracket '%c'\n",
                           line, ch);
                }
            }

            continue;
        }

        /* Special characters */
        if (ch == ';' || ch == ',')
        {
            printf("%c : Special character\n", ch);
            continue;
        }

        /* Invalid characters */
        if (ch == '@' || ch == '$')
        {
            printf("Line %d: Error: Invalid character: %c\n",
                   line, ch);
            continue;
        }

        /* Other characters */
        printf("%c : Special character\n", ch);
    }

    /* Missing opening bracket matches */
    while (top >= 0)
    {
        if (stack[top] == '(')
        {
            printf("Line %d: Error: Missing ')'\n",
                   stack_line[top]);
        }
        else if (stack[top] == '{')
        {
            printf("Line %d: Error: Missing '}'\n",
                   stack_line[top]);
        }
        else if (stack[top] == '[')
        {
            printf("Line %d: Error: Missing ']'\n",
                   stack_line[top]);
        }

        top--;
    }

    fclose(fp);
}