#include"shell_prompt.h"
#include<stdio.h>
#include<stdlib.h>
#include"lexer.h"
#include"grammar.h"
int main(){
    home();
    char* line=NULL;
    size_t l=0;
    ssize_t r;
    while(1){
        prompt();
        r=getline(&line,&l,stdin);
        if (r==-1){
            printf("\n");break;

        }
        if (r>0&&line[r-1]=='\n'){line[r-1]='\0';r--;}
        if (r==0)continue;
        Token*tokens=convert(line);
        if (tokens){if (check(tokens)==0){
            fprintf(stderr, "cshell: invalid syntax\n");freee(tokens);}}
    }
    free(line);
    return 0;
}