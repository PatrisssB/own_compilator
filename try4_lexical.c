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
    ID,
    END,
    CT_INT,
    CT_REAL,
    CT_CHAR,
    CT_STRING,
    // Add more token codes as needed
}; // tokens codes

typedef struct _Token 
{
    int code;      // code (name);
    union 
    {
        char *text; // used for ID, CT_STRING (dynamically allocated)
        long int i; // used for CT_INT, CT_CHAR
        double r;   // used for CT_REAL
    };
    int line;       // the input file line
    struct _Token *next; // link to the next token
} Token;

Token *tokens = NULL;    // Global variable to store tokens
Token *lastToken = NULL; // Global variable to store last token
int line = 1;            // Global variable to track current line number

void addTk(int code) 
{
    Token *tk;
    SAFEALLOC(tk, Token);
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastToken) {
        lastToken->next = tk;
    } else {
        tokens = tk;
    }
    lastToken = tk;
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

//  if a token is a keyword ? is needed? 
int checkKeyword(const char *text) 
{
    if (strcmp(text, "break") == 0) return BREAK;
    if (strcmp(text, "char") == 0) return CHAR;
    // Add more keyword checks as needed
    return ID; // If not a keyword, treat it as an identifier
}

// lexical analyzer
int getNextToken(const char *input) 
{
    int state = 0, nextCh;
    char ch;
    const char *pStartCh = input;
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
            else if (isdigit(ch)) 
            {
                pStartCh = input; //CT_INT base 10
                input++;         
                state = 4;       
            } 

            else if (ch == ' 0 ') 
            {
                pStartCh = input; //CT_INT base 8
                input++;         
                state = 5;        
            } 
    
            else if (ch == ' \' ') 
            {
                pStartCh = input;  //CT_CHAR
                input++;         
                state = 16;      
            } 
            
            else if (ch == '"') 
            {
                pStartCh = input; //CT_STRING
                input++;         // consume the character
                state = 13;       // set the new state for string constant

            /* MORE STATES FROM 0 NEED TO BE ADDED    

            } else if (ch == '=') {
                input++;
                state = 17;
            } else if (ch == ';') {
                input++;
                state = 18;
            } else if (ch == ' ' || ch == '\r' || ch == '\t') {
                input++; // consume the character and remains in state 0
            } else if (ch == '\n') { // handled separately in order to update the current line
                line++;
                input++;
            }
            */
            
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
                addTk(ID);
                SAFEALLOC(tk->text, char);
                tk->text = strndup(pStartCh, nextCh);
            }
            return lastToken->code;

        case 3:
        
            //here i am confused as fuck
            //theoreticall from 3 & 5 i need to go in 8 or 9?
            if (isdigit(ch))
                input++;
            else if( ch == " . ")
                state = 10;
            else if( ch == "e" || ch == "E")
                state = 13;
            else
                state = 4 //final state for CT_INT
        
        break;
        
        case 4:
        
            nextCh = input - pStartCh; // the integer constant length
            // it's an integer constant
            addTk(CT_INT);
            SAFEALLOC(tk->text, char);
            tk->text = strndup(pStartCh, nCh);
            return lastToken->code;


        case 5:
        
            if (isdigit(ch) && (ch >= '0' && ch <= '7')) 
            {
                input++;
            } 
            else if( ch == 'x' )
            {
                state = 6; 
            }
            else
                state 4;

        break;

        case 6:
            if(isalnum(ch))
                state = 7;

        break;


        case 7:

            if (isalnum(ch))
                input++;
            else
                state = 4;

        break;

        case 12:

            nextCh = input - pStartCh; // the real constant length
            addTk(CT_REAL);
            SAFEALLOC(tk->text, char);
            tk->text = strndup(pStartCh, nCh);
            return lastToken->code;

        case 16:

            if (ch != '\\' && ch != '\'')
                input++;
            else if (ch == '\\') 
            {
                input += 2; // skip over escape sequence
            } else
                state = 11;

            break;
        case 11:

            if (ch == '\'') 
            {
                input++;
                state = 12;
            } else
                tkerr(lastToken, "invalid character constant");

            break;
        case 12:

            // it's a character constant
            addTk(CT_CHAR);
            SAFEALLOC(tk->text, char);
            tk->text = strndup(pStartCh, input - pStartCh);
            return lastToken->code;

        case 13:

            if (ch != '\\' && ch != '"')
                input++;
            else if (ch == '\\') {
                input += 2; // skip over escape sequence
            } else
                state = 14;

            break;
        case 14:

            if (ch == '"') {
                input++;
                state = 15;
            }

            break;
        case 15:

            // it's a string constant
            addTk(CT_STRING);
            SAFEALLOC(tk->text, char);
            tk->text = strndup(pStartCh, input - pStartCh);
            return lastToken->code;

        case 17:

            if (ch == '=')
                state = 19;
            else
                state = 20;

            break;
        case 19:

            addTk(ASSIGN);
            return lastToken->code;

        case 20:

            addTk(EQUAL);
            return lastToken->code;

        case 18:

            addTk(SEMICOLON);
            return lastToken->code;
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
    if (argc != 2) 
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) 
    {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *input = (char *)malloc(size + 1);
    fread(input, sizeof(char), size, file);
    input[size] = '\0';

    fclose(file);

    int token;
    while ((token = getNextToken(input)) != END) 
    {
        // Process tokens if needed
    }

    printTokens(tokens);

    free(input);
    return 0;
}
