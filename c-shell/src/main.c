#include"shell_prompt.h"
#include<stdio.h>
#include<stdlib.h>
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
    }
    free(line);
    return 0;
}