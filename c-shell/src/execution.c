#include"execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/limits.h>

static int isfile(char*path){
     struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISREG(st.st_mode) && access(path, X_OK) == 0) {
            return 1;
        }
    }    
    return 0;
}
static char* path(char*cmd){
int t=0;char*search=cmd;
if (cmd[0]=='%'){
    t=1;search=cmd+1;
}
if (!t&&strchr(cmd,'/')!=NULL){
    if (isfile(search)){return strdup(search);}
    return NULL;
}
char cwd[PATH_MAX];
char temp[PATH_MAX];
if (!t){
    if (getcwd(cwd,PATH_MAX)!=NULL){
        strcpy(temp,cwd);strcat(temp,"/");strcat(temp,search);
        if (isfile(temp)){return strdup(temp);}
    }
}
 char*path=getenv("PATH");
        if (path!=NULL){
          char* copy=(char*)malloc(strlen(path)+1);
          strcpy(copy,path);
          char* dir=strtok(copy,":");
          while(dir!=NULL){
            int n=strlen(dir);
            if(dir[n-1]=='/'){
                strcpy(temp,dir);strcat(temp,search);
            }
            else {
                                strcpy(temp,dir);strcat(temp,"/");strcat(temp,search);
            }
            if (isfile(temp)){
                free(copy);return strdup(temp);
            }
            dir=strtok(NULL,":");
          }  
          free(copy);
}
return NULL;}

void execute(char**input,int count){
    if (input[0]==NULL||count==0)return;
    char* cmd=input[0];
    char* p=path(cmd);
    if (p==NULL){
        if (cmd[0]=='%'){
            fprintf(stderr,"cshell: command not found (%s)\n",cmd+1);

        }
        else {fprintf(stderr,"cshell: command not found (%s)\n",cmd);}
        return;
    }
    if (input[0][0]=='%'){input[0]+=1;}
    char **exec=malloc(sizeof(char*)*(count+1));
    for (int i=0;i<count;i++){
        exec[i]=input[i];
    }
    exec[count]=NULL;
    pid_t pid = fork();
    if (pid< 0) {
        perror("fork error");
    } 
    else if (pid == 0) {
        execv(p, exec);
        perror("execv error");
        exit(EXIT_FAILURE);
    } 
    else {
        waitpid(pid, NULL, 0);
    }
    free(p);free(exec);
}