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
    RETURN,
    DOUBLE,
    STRUCT,
    WHILE,
    BREAK,
    ELSE,
    VOID,
    CHAR,
    FOR,
    INT,
    IF,

    // Add more token codes as needed
}; 

typedef struct _Token{
    int code; // code (name)
    union{
        char *text; // used for ID, CT_STRING (dynamically allocated)
        long int i; // used for CT_INT, CT_CHAR
        double r; // used for CT_REAL
        };
    int line; // the input file line
    struct _Token *next; // link to the next token
}Token;

Token *tokens = NULL;    // Global variable to store tokens
Token *lastToken = NULL; // Global variable to store last token
int line = 1;            // Global variable to track current line number


Token *addTk(int code){
    Token *tk;
    SAFEALLOC(tk,Token)
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(lastToken){
        lastToken->next=tk;
     }else{
        tokens=tk;
     }
    lastToken=tk;
    return tk;
}

void err(const char *fmt,...){
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk,const char *fmt,...){
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

int getNextToken(const char *input){
    int state=0,nCh;
    char ch;
    const char *pStartCh;
    Token *tk;
    while(1){ // infinite loop
        ch= *input;
        switch(state){
            case 0 :
                if(isalpha(ch)||ch=='_'){
                pStartCh=input; // memorizes the beginning of the ID
                input++; // consume the character
                state=1; // set the new state
                }
                else if(ch=='='){
                printf("egal");
                input++;
                state=3;
                }
                else if(ch==' '||ch=='\r'||ch=='\t'){
                input++; // consume the character and remains in state 0
                }
                else if(ch=='\n'){ // handled separately in order to update the current line
                line++;
                input++;
                }
                else if(ch==0){ // the end of the input string
                addTk(END);
                return END;
                }
                else if(isdigit(ch) && ch >= '1' && ch <= '9'){
                    pStartCh=input; 
                    input++; 
                    state=3;
                }
                else if(ch=='0'){ // the end of the input string
                    pStartCh=input; 
                    input++; 
                    state=5;
                }
                else if(ch=='\''){
                    pStartCh=input;
                    input++;
                    state=14;

                }
                else tkerr(addTk(END),"invalid character");
                break;
            case 1 :
                if(isalnum(ch)||ch=='_'){
                    input++;
                    
                }
                else {

                    state=2;
                }
                break;
                

            case 2:
                nCh=input-pStartCh; // the id length
                // keywords tests
                 if (nCh == 2 && !memcmp(pStartCh, "if", 2))
                addTk(IF);
            else if (nCh == 3 && !memcmp(pStartCh, "int", 3))
                addTk(INT);
            else if (nCh == 3 && !memcmp(pStartCh, "for", 3))
                addTk(FOR);
            else if (nCh == 4 && !memcmp(pStartCh, "char", 4))
                addTk(CHAR);
            else if (nCh == 4 && !memcmp(pStartCh, "void", 4))
                addTk(VOID);
            else if (nCh == 4 && !memcmp(pStartCh, "else", 4))
                addTk(ELSE);
            else if (nCh == 5 && !memcmp(pStartCh, "break", 5))
                addTk(BREAK);
            else if (nCh == 5 && !memcmp(pStartCh, "while", 5))
                addTk(WHILE);
            else if (nCh == 6 && !memcmp(pStartCh, "struct", 6))
                addTk(STRUCT);
            else if (nCh == 6 && !memcmp(pStartCh, "double", 6))
                addTk(DOUBLE);
            else if (nCh == 6 && !memcmp(pStartCh, "struct", 6))
                addTk(RETURN);
                
            else 
            {
                
                // if no keyword, then it is an ID
                tk = addTk(ID);
                tk->text = strndup(pStartCh, nCh);
                
            }
             return tk->code; 
           case 3:
               if(isdigit(ch))
                   input++;
                else if(ch=='e'||ch=='E'){
                    input++;
                    state=10;
                }
                else if(ch=='.'){
                    input++;
                    state=8;
                }
                else 
                    state=4;
            break;

            case 4: 
                tk = addTk(CT_INT);
                tk->text = strndup(pStartCh, nCh);    
            return tk->code;   

            case 5:
                if(isdigit(ch) && ch <= '7')
                   input++;
                else if(ch=='8'||ch=='9')
                    state=3;
                else if(ch == 'x'){
                    input++;
                    state=6;
                }
                else if(ch=='.'){
                    input++;
                    state=8;
                }
                else if(ch=='e'||ch=='E'){
                    input++;
                    state=10;
                }
                else 
                    state=4;
             break;

            case 6:
                if ((ch >= '0' && ch <= '9') || ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))){
                    input++;
                    state=7;
                }else {
                    // Handle invalid character after '0x'
                    tkerr(addTk(END), "invalid hexadecimal digit after '0x'");
                    return -1; // Return error code
                }
            break;
            case 7:
                 if ((ch >= '0' && ch <= '9') || ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))){
                    input++; 
                }
                else {

                    state=4;
                }
            break;

            case 8:
                if(isdigit(ch)){
                    input++;
                    state=9;
                }
                else
                  state=13;
            break;

            case 9:
                if(isdigit(ch))
                   input++;
                else if(ch=='e'||ch=='E'){
                    input++;
                    state=10;
                }
            break;

            case 10:
                if(isdigit(ch)){
                    input++;
                    state=12;
                }
                else if(ch=='-'||ch=='+'){
                    input++;
                    state==11;
                }
            break;

            case 11:
                if(isdigit(ch)){
                    input++;
                    state=12;
                }
            break;
                
            case 12:
                if(isdigit(ch))
                   input++;
                else 
                    state=13;
            break;

            case 13:
                tk = addTk(CT_REAL);
                tk->text = strndup(pStartCh, nCh);    
            return tk->code; 

            case 14:
                if(ch=='//'){
                    input++;
                    state=15;
                }
                else if(ch=='^'||ch=='\''||ch=='\\'){
                    input++;
                    state=18;
                }
            break;
            
            case 15:
                if(ch=='a'||ch=='b'||ch=='f'||ch=='m'||ch=='n'||ch=='t'||ch=='v'||ch=='\''||ch=='?'||ch=='"'||ch=='\\'||ch=='0'){
                    input++;
                    state=16;
                }
            break;

            case 16:
                if(ch=='\''){
                    input++;
                    state=17;
                }
            break;

            case 17:
               tk = addTk(CT_CHAR);
                tk->text = strndup(pStartCh, nCh);    
            return tk->code;

            case 18:
                 if(ch=='\''){
                    input++;
                    state=17;
                }
            break;


                
                 
              
        }
    }

}


void printTokens(Token *tokens){
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
    if (argc <2) 
    {
        printf("Usage: %s <input_string>\n", argv[0]);
        return 1;
    }

   /* FILE *file = fopen(argv[1], "r");
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
    */
    int token;
    
    for (int i = 1; i<argc; i++){
    token = getNextToken(argv[i]);
        switch (token) 
        {
            case ID:
                printf("Identifier: %s\n", lastToken->text);
                break;
            case CT_INT:
                printf("INT number: %s\n", lastToken->text);
                break;
            case CT_REAL:
                printf("REAL number: %s\n", lastToken->text);
                break;
            case CT_CHAR:
                printf("CHAR: %s\n", lastToken->text);
                break;

            default:
                printf("Token code: %d\n", token);
        }
    }

    printTokens(tokens);

    //free(input);
    return 0;
}