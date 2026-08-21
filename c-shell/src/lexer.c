#include"lexer.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>
void freee(Token *head) {
    Token*curr=head;
    while (curr!=NULL) {
        Token *next=curr->next;
        if (curr->val) free(curr->val);
        free(curr);
        curr=next;
    }
}
Token* add(Token**head,Token**tail,Types type,char*val){
    Token *new=malloc(sizeof(Token));
    if (!new){perror("malloc failed");exit(1);}
    new->type=type;new->val=val!=NULL?strdup(val):NULL;
    new->next=NULL;
if (*head==NULL){*head=new;}
else {(*tail)->next=new;}
*tail=new;return new;
}
Token*convert(char*input){
    Token*head=NULL;
    Token*tail=NULL;
    char*p=input;while(*p!='\0'){
        if (isspace(*p)){p++;continue;}
        else {
        if (*p=='|'){add(&head,&tail,PIPE,NULL);p++; continue;}
        if (*p=='&'){add(&head,&tail,AND,NULL);p++; continue; }
        if (*p==';'){add(&head,&tail,COLON,NULL); p++; continue; }
        if (*p=='<'){add(&head,&tail,LESS, NULL);p++; continue; }
        if (*p=='>') {if (*(p+1)=='>') {
                add(&head, &tail,GREATER2,NULL);
                p+=2;
            } else {
                add(&head,&tail,GREATER,NULL);
                p++;
            }
            continue;}
        }
char buffer[4096];
int b=0;int singleq=0,doubleq=0;
while(*p!='\0'){
    if (!singleq&&!doubleq){
        if (isspace(*p)||strchr("|&<>;",*p))break;
    }
    if (*p=='\\'&&!singleq){
        if (*(p+1)=='\0'){fprintf(stderr,"cshell: invalid syntax\n");freee(head);return NULL;}
    
    if (doubleq){
        if (*(p+1)=='"'||*(p+1)=='\\') {
            buffer[b++]=*(p+1); p += 2;
                    }
        else {buffer[b++] = '\\';buffer[b++]= *(p+1);p+= 2;
                    }}
        else {buffer[b++]=*(p+1);p+=2;
            }
                }
            else if (*p =='\'') {
                if (!doubleq) singleq = !singleq;
                else buffer[b++]=*p;
                p++;
            } else if (*p =='"') {
                if (!singleq) doubleq = !doubleq;
                else {buffer[b++] = *p;} p++;
            } else {
                buffer[b++] = *p;
                p++;
            }
        }if (singleq||doubleq){fprintf(stderr,"cshell: invalid syntax\n");
            freee(head);
            return NULL;}
            buffer[b]='\0';
            add(&head,&tail,WORD,buffer);
    }
    return head;
}

