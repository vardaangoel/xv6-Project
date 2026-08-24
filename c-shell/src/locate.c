#include "locate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>

static int isfile(char* path){
   struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISREG(st.st_mode) && access(path, X_OK) == 0) {
            return 1;
        }
    }    
    return 0; } 

void locate(char**input,int count){
    if (count==0){
        fprintf(stderr, "locate: invalid syntax\n");return;
    }
    char cwd[PATH_MAX];
    if (getcwd(cwd,PATH_MAX)==NULL) {
        perror("locate error");
        return;
    }
    char*path=getenv("PATH");
    for (int i=0;i<count;i++){
        char*inp=input[i];
        int num=0;
        char p[PATH_MAX];
        if (strcmp(cwd,"/")==0){strcpy(p,"/");strcat(p,inp);}
        else{strcpy(p,cwd);strcat(p,"/");strcat(p,inp);}
        if (isfile(p)){
            printf("%s\n",p);
            num++;
        }
        if (path!=NULL){
          char* temp=(char*)malloc(strlen(path)+1);
          strcpy(temp,path);
          char* dir=strtok(temp,":");
          while(dir!=NULL){
            int n=strlen(dir);
            if(dir[n-1]=='/'){
                strcpy(p,dir);strcat(p,inp);
            }
            else {
                                strcpy(p,dir);strcat(p,"/");strcat(p,inp);
            }
            if (isfile(p)){
                printf("%s\n",p);num++;
            }
            dir=strtok(NULL,":");
          }  
          free(temp);
        }
        if (num==0){
            fprintf(stderr,"locate: command not found (%s)\n",inp);
        }
    }
}