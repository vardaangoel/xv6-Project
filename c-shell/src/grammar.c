#include"grammar.h"
#include<stdio.h>
int check(Token* head){
    if (head==NULL){return 1;}
    if (head->type!=WORD){return 0;}
    Token* curr=head->next;
    while(curr!=NULL){
if (curr->type==AND){if (curr->next!=NULL&&curr->next->type!=WORD)return 0;
}
    
    else if (curr->type==WORD){curr=curr->next;continue;}
    else {
        if (curr->next==NULL||curr->next->type!=WORD){return 0;}
    }
    curr=curr->next;
}return 1;
}