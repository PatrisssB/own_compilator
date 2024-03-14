#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//codes
enum 
{
    ID,
    CT_INT,
    CT_REAL,
    CT_CHAR,
    CT_STRING
};

//build of the stucture of the Token
typedef struct 
{
    int code;
    char *value; //if they need to store a value eq ID
} Token;

//add token to list
void addToken(Token **tokens, int code, const char *value) 
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->code = code;
    token->value = strdup(value); //duplicate the string to ensure it's not modified externally
    *tokens = token;
}

//free memory 
void freeTokens(Token *tokens) 
{
    Token *temp;
    while (tokens != NULL) 
    {
        temp = tokens;
        tokens = tokens->next;
        free(temp->value);
        free(temp);
    }
}

// lexical analyzer 
Token* lex(const char *input) //we take the imput from a file
{
    Token *tokens = NULL;
    int len = strlen(input);
    int i = 0;

    while (i < len) 
    {
        //skip whitespaces
        if (isspace(input[i])) 
        {
            i++;
            continue;
        }
        //check for identifiers
        if (isalpha(input[i]) || input[i] == '_') 
        {
            int j = i;
            while (isalnum(input[j]) || input[j] == '_') 
            {
                j++;
            }
            char *identifier = strndup(input + i, j - i);
            i = j;
            addToken(&tokens, ID, identifier);
            continue;
        }
        // Check for integer constants
        if (isdigit(input[i])) 
        {
            int j = i;
            while (isdigit(input[j])) 
            {
                j++;
            }
            char *number = strndup(input + i, j - i);
            i = j;
            addToken(&tokens, CT_INT, number);
            continue;
        }
        // Check for real constants
        if (input[i] == '.') {
            int j = i + 1;
            while (isdigit(input[j])) {
                j++;
            }
            char *number = strndup(input + i, j - i);
            i = j;
            addToken(&tokens, TK_CT_REAL, number);
            continue;
        }
        // Check for character constants
        if (input[i] == '\'') {
            int j = i + 1;
            while (input[j] != '\'') {
                if (input[j] == '\0') {
                    printf("Error: Unterminated character constant\n");
                    freeTokens(tokens);
                    return NULL;
                }
                j++;
            }
            char *ch = strndup(input + i, j - i + 1);
            i = j + 1;
            addToken(&tokens, TK_CT_CHAR, ch);
            continue;
        }
        // Check for string constants
        if (input[i] == '"') {
            int j = i + 1;
            while (input[j] != '"') {
                if (input[j] == '\0') {
                    printf("Error: Unterminated string constant\n");
                    freeTokens(tokens);
                    return NULL;
                }
                j++;
            }
            char *str = strndup(input + i, j - i + 1);
            i = j + 1;
            addToken(&tokens, TK_CT_STRING, str);
            continue;
        }
        printf("Error: Invalid character '%c'\n", input[i]);
        freeTokens(tokens);
        return NULL;
    }
    addToken(&tokens, TK_EOF, "EOF");
    return tokens;
}

// Function to print tokens
void printTokens(Token *tokens) {
    while (tokens != NULL) {
        printf("Token: %d", tokens->code);
        if (tokens->value != NULL) {
            printf(" (%s)", tokens->value);
        }
        printf("\n");
        tokens = tokens->next;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
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

    Token *tokens = lex(input);
    if (tokens != NULL) {
        printTokens(tokens);
        freeTokens(tokens);
    }

    free(input);
    return 0;
}
