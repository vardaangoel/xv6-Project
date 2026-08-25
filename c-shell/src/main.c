#include"shell_prompt.h"
#include<stdio.h>
#include<stdlib.h>
#include"lexer.h"
#include"grammar.h"
#include"hop.h"
#include <sys/types.h>
#include<string.h>
#include"reveal.h"
#include"peek.h"
#include"locate.h"
#include"execution.h"
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
            fprintf(stderr, "cshell: invalid syntax\n");freee(tokens);}
            else{
                if (tokens->type==WORD&&strcmp(tokens->val,"hop")==0){
                    char*inp[256];
                    int cnt=0;
                    Token*curr=tokens->next;
                    while(curr!=NULL&&curr->type==WORD&&cnt<256){
                        inp[cnt++]=curr->val;
                        curr=curr->next;
                    }
                    hopper(inp,cnt);
                }
                else if (tokens->type==WORD&&strcmp(tokens->val,"reveal")==0){
                    char*input[256];int count=0;
                    Token*curr=tokens->next;
                   while (curr != NULL && curr->type == WORD && count < 256) {
                        input[count++] = curr->val;
                        curr = curr->next;
                    }
                    
                    reveal(input, count);
                }
                else if (tokens->type==WORD&&strcmp(tokens->val,"peek")==0){
                    char*input[256];int count=0;
                    Token*curr=tokens->next;

                     while (curr != NULL && curr->type == WORD && count < 256) {
                        input[count++] = curr->val;
                        curr = curr->next;
                    }
peek(input,count);
                }
                else if(tokens->type==WORD&&strcmp(tokens->val,"locate")==0){
                     char*input[256];int count=0;
                    Token*curr=tokens->next;

                     while (curr != NULL && curr->type == WORD && count < 256) {
                        input[count++] = curr->val;
                        curr = curr->next;
                    }locate(input,count);
                }
                else if (tokens->type==WORD){
                    char*input[256];int count=0;
                                Token*curr=tokens;

                     while (curr != NULL && curr->type == WORD && count < 256) {
                        input[count++] = curr->val;
                        curr = curr->next;
                    }execute(input,count);
                }
                }
            
            free(tokens);
    }}
    free(line);
    return 0;
}