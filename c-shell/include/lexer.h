#ifndef LEXER
#define LEXER
typedef enum{
    WORD,PIPE,AND,COLON,LESS,GREATER,GREATER2
}Types;

typedef struct Token{
Types type;char*val;struct Token*next;
}Token;

Token* convert(char* input);
void print(Token* head);
void freee(Token*head);
#endif