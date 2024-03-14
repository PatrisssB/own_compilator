#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//codes
enum 
{
    TK_IDENTIFIER,
    TK_NUMBER,
    TK_STRING,
    TK_CHAR,
    TK_INT,
    TK_FLOAT,
    TK_VOID,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,
    TK_EQUAL,
    TK_SEMICOLON,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_COMMA
};

// Token structure
typedef struct {
    int code;
    char *value; // For tokens that need to store a value (like identifiers, numbers, strings, etc.)
} Token;

// Function to add a token to the list
void addToken(Token **tokens, int code, const char *value) {
    Token *token = (Token *)malloc(sizeof(Token));
    token->code = code;
    token->value = strdup(value); // Duplicate the string to ensure it's not modified externally
    *tokens = token;
}

// Function to free memory occupied by tokens
void freeTokens(Token *tokens) {
    Token *temp;
    while (tokens != NULL) {
        temp = tokens;
        tokens = tokens->next;
        free(temp->value);
        free(temp);
    }
}

// Lexical analyzer function
Token* lex(const char *input) {
    Token *tokens = NULL;
    int len = strlen(input);
    int i = 0;

    while (i < len) {
        // Skip whitespaces
        if (isspace(input[i])) {
            i++;
            continue;
        }
        // Check for identifiers
        if (isalpha(input[i]) || input[i] == '_') {
            int j = i;
            while (isalnum(input[j]) || input[j] == '_') {
                j++;
            }
            char *identifier = strndup(input + i, j - i);
            i = j;
            // Check for keywords
            if (strcmp(identifier, "int") == 0) {
                addToken(&tokens, TK_INT, identifier);
            } else if (strcmp(identifier, "float") == 0) {
                addToken(&tokens, TK_FLOAT, identifier);
            } else if (strcmp(identifier, "void") == 0) {
                addToken(&tokens, TK_VOID, identifier);
            } else if (strcmp(identifier, "if") == 0) {
                addToken(&tokens, TK_IF, identifier);
            } else if (strcmp(identifier, "else") == 0) {
                addToken(&tokens, TK_ELSE, identifier);
            } else if (strcmp(identifier, "while") == 0) {
                addToken(&tokens, TK_WHILE, identifier);
            } else if (strcmp(identifier, "for") == 0) {
                addToken(&tokens, TK_FOR, identifier);
            } else if (strcmp(identifier, "return") == 0) {
                addToken(&tokens, TK_RETURN, identifier);
            } else {
                addToken(&tokens, TK_IDENTIFIER, identifier);
            }
            continue;
        }
        // Check for numbers
        if (isdigit(input[i])) {
            int j = i;
            while (isdigit(input[j])) {
                j++;
            }
            char *number = strndup(input + i, j - i);
            i = j;
            addToken(&tokens, TK_NUMBER, number);
            continue;
        }
        // Check for operators and punctuation
        switch (input[i]) {
            case '+':
                addToken(&tokens, TK_PLUS, "+");
                break;
            case '-':
                addToken(&tokens, TK_MINUS, "-");
                break;
            case '*':
                addToken(&tokens, TK_STAR, "*");
                break;
            case '/':
                addToken(&tokens, TK_SLASH, "/");
                break;
            case '=':
                addToken(&tokens, TK_EQUAL, "=");
                break;
            case ';':
                addToken(&tokens, TK_SEMICOLON, ";");
                break;
            case '(':
                addToken(&tokens, TK_LPAREN, "(");
                break;
            case ')':
                addToken(&tokens, TK_RPAREN, ")");
                break;
            case '{':
                addToken(&tokens, TK_LBRACE, "{");
                break;
            case '}':
                addToken(&tokens, TK_RBRACE, "}");
                break;
            case ',':
                addToken(&tokens, TK_COMMA, ",");
                break;
            default:
                printf("Error: Invalid character '%c'\n", input[i]);
                freeTokens(tokens);
                return NULL;
        }
        i++;
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
