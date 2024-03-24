#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define SAFEALLOC(var, Type) \
    if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
        err("not enough memory");

enum
{
    END,
    ID,
    CT_INT,
    CT_REAL,
    CT_CHAR,
    CT_STRING,
    IF,
    INT,
    FOR,
    VOID,
    ELSE,
    CHAR,
    DOUBLE,
    RETURN,
    BREAK,
    WHILE,
    STRUCT,
    DOT,
    DIV,
    MUL,
    SUB,
    ADD,
    LACC,
    RACC,
    LBRACKET,
    RBRACKET,
    LPAR,
    RPAR,
    SEMICOLON,
    COMMA,
    AND,
    OR,
    NOT,
    ASSIGN,
    EQUAL,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,
    COMMENT,
    LINE_COMMENT,
    SPACE
    //all the token were added 
}; // tokens codes

typedef struct _Token
{
    int code;             // code (name);
    union
    {
        char *text;      // used for ID, CT_STRING (dynamically allocated)
        long int i;      // used for CT_INT, CT_CHAR
        double r;        // used for CT_REAL
    };
    int line;             // the input file line
    struct _Token *next;  // link to the next token
} Token;

Token *tokens = NULL;       // Global variable to store tokens
Token *lastToken = NULL;    // Global variable to store last token
int line = 1;               // Global variable to track current line number

Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk, Token);
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    tk->text = NULL; // Initialize text pointer to NULL
    if (lastToken)
    {
        lastToken->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastToken = tk;
    return tk;
}

void err(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

// lexical analyzer
int getNextToken(const char *input)
{
    int state = 0;
    int nextCh;
    char ch;
    const char *pStartCh;
    Token *tk;

    while (1)
    { // infinite loop
        ch = *input;
        switch (state)
        {
        case 0: // transitions test for state 0

            if (isalpha(ch) || ch == '_') //ID
            {
                pStartCh = input; // memorizes the beginning of the ID
                input++;         // consume the character
                state = 1;        // set the new state
            }
            else if (isdigit(ch) && ch >= '1' && ch <= '9')
            {
                pStartCh = input; //CT_INT base 10
                input++;
                state = 4;
            }

            else if (ch == '0')
            {
                pStartCh = input; //CT_INT base 8
                input++;
                state = 5;
            }

            else if (ch == '\'')
            {
                pStartCh = input; //CT_CHAR
                input++;
                state = 16;
            }

            else if (ch == '"')
            {
                pStartCh = input; //CT_STRING
                input++;
                state = 13;
            }
            /* MORE STATES FROM 0 NEED TO BE ADDED */
            else if (ch == 0)
            { // the end of the input string
                addTk(END);
                return END;
            }
            else
                tkerr(lastToken, "invalid character");
            break;

        case 1:

            if (isalnum(ch) || ch == '_')
                input++; //loop with itself
            else
                state = 2; //which is the final state for ID
            break;

        case 2:

            nextCh = input - pStartCh; // the length of ID
            // keywords tests
            if (nextCh == 2 && !memcmp(pStartCh, "if", 2))
                addTk(IF);
            else if (nextCh == 3 && !memcmp(pStartCh, "int", 3))
                addTk(INT);
            else if (nextCh == 3 && !memcmp(pStartCh, "for", 3))
                addTk(FOR);
            else if (nextCh == 4 && !memcmp(pStartCh, "char", 4))
                addTk(CHAR);
            else if (nextCh == 4 && !memcmp(pStartCh, "void", 4))
                addTk(VOID);
            else if (nextCh == 4 && !memcmp(pStartCh, "else", 4))
                addTk(ELSE);
            else if (nextCh == 5 && !memcmp(pStartCh, "break", 5))
                addTk(BREAK);
            else if (nextCh == 5 && !memcmp(pStartCh, "while", 5))
                addTk(WHILE);
            else if (nextCh == 6 && !memcmp(pStartCh, "struct", 6))
                addTk(STRUCT);
            else if (nextCh == 6 && !memcmp(pStartCh, "double", 6))
                addTk(DOUBLE);
            else if (nextCh == 6 && !memcmp(pStartCh, "struct", 6))
                addTk(RETURN);

            else
            {
                // if no keyword, then it is an ID
                tk = addTk(ID);
                //SAFEALLOC(tk->text, char);
                tk->text = strndup(pStartCh, nextCh);
            }
            return tk->code;

        case 3:
            if (isdigit(ch))
            {
                input++;
                //base 10 case for CT_INT
            }
            else if( ch == '0')
            {
                input++;
                state = 9;
            }
            else if (tolower(ch) == 'e' || toupper(ch) == 'E')
            {
                input++;
                state = 13;
            }
            else if( ch == '.')
            {
                input++;
                state = 10;
            }
            else
            {
                state = 4; //final state for CT_INT
            }
        break;

        case 4:

            //nextCh = input - pStartCh; // the integer constant length
            // it's an integer constant
            addTk(CT_INT);
            //SAFEALLOC(tk->text, char);
            tk->text = strndup(pStartCh, nextCh);
            return tk->code;

        case 5:

            if (isdigit(ch) && (ch >= '0' && ch <= '7'))
                input++;
                //case for base 8 for CT_INT
            else if (ch == '8' || ch == '9')
            {
                //input ++;
                state = 8;
            }
            else if (ch == 'x')
            {
                input++;
                state = 6;
            }
            else if( ch == '.')
            {
                input ++;
                state = 13; //or state 10?
            }
            else if(ch == 'e' || ch == 'E' )
            {
                input ++;
                state = 13;
            }
            else
                state = 4;
        break;

        case 6:
            if ((ch >= '0' && ch <= '9') || ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')))
            {
                input++; //check this
                state = 7;
            }
            else
            {
                //invalid character for base 16: 0x ... 
                tkerr(addTk(END), "hexadecimal character incomplete/ written wrong 0x...";
                return -1;
            }
        break;

        case 7:
            if ((ch >= '0' && ch <= '9') || ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')))
            {
                input++;
                
            }
            else 
            {
                state = 4;
            }
        break; 

        case 8:
            if (isdigit(ch))
            {
                input++;
                state = 9;
            }
            else if (ch=='e'||ch=='E')
            {
                input++;
                state = 13;
            }
        break;

        case 9:
            if(isdigit(ch))
                   input++;
            else if (ch == 'e' || ch == 'E' )
            {
                input ++;
                state = 13;
            }
            else if (ch == '.')
            {
                input ++;
                state = 10;
            }
        break;

        case 10:
            if(isdigit(ch))
            {
                input++;
                state = 11;
            }
        break;

        case 11:
            if(isdigit(ch))
            {
                input++;
            }
            else if( ch == 'e' || ch == 'E' )
            {
                input++;
                state = 13;
            }
            else
            {
                //CT_REAL 
                state = 12;
            }
        break;

        case 12:
            tk = addTk(CT_REAL);
            tk->text = strndup(pStartCh, nextCh);    
        return tk->code; 

        case 13:
            if( ch == '-' || ch == '+' )
            {
                input ++;
                state = 14;
            }
            else
                state = 15;
        break;

        case 14:
            if(isdigit(ch))
            {
                state = 15;
            }
        break;

        case 15:
            if(isdigit(ch))
            {
                    input ++;
                    state = 12;
            }
        break;

        case 16:
            if( ch == '\'')
            {
                input ++;
                state = 17;
            }
        break;

        case 17:
            if(ch=='^'||ch=='\''||ch=='\\'){
                    input++;
                    state=18;
        
        }
    }
}

// Function to print tokens
void printTokens(Token *tokens)
{
    while (tokens != NULL)
    {
        printf("Token: %d", tokens->code);
        if (tokens->text != NULL)
        {
            printf(" (%s)", tokens->text);
        }
        printf("\n");
        tokens = tokens->next;
    }
}

int main(int argc, char *argv[])
{
    // Define a hardcoded string
    const char *inputString = "ana123";

    int token;
    const char *p = inputString; // Pointer to the current character in the input string
    while ((token = getNextToken(p)) != END && *p != '\0')
    {
        // Move to the next character in the input string
        p++;
    }

    if (lastToken != NULL)
    {
        switch (lastToken->code)
        {
        case ID:
            if (lastToken->text != NULL)
            {
                printf("Identifier: %s\n", lastToken->text);
            }
            break;
        case CT_INT:
            if (lastToken->text != NULL)
            {
                printf("Integer Constant: %s\n", lastToken->text);
            }
            break;
        case CT_REAL:
            if (lastToken->text != NULL)
            {
                printf("Real Constant: %s\n", lastToken->text);
            }
            break;
        case CT_CHAR:
            if (lastToken->text != NULL)
            {
                printf("Character Constant: %s\n", lastToken->text);
            }
            break;
        case CT_STRING:
            if (lastToken->text != NULL)
            {
                printf("String Constant: %s\n", lastToken->text);
            }
            break;
        //cases for other token types as needed
        default:
            printf("Token code: %d\n", lastToken->code);
        }
    }

    return 0;
}
