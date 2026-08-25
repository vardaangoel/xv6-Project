#include"execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <fcntl.h>

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
            if(n>0&&dir[n-1]=='/'){
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
    char*arg[1000];char*files[1000];int file_count=0;
    int num=0;
    for (int i=0;i<count;i++){
        if (strcmp(input[i],"<")==0){
            if (i+1>=count){fprintf(stderr,"cshell: syntax error\n");return;}
            else {files[file_count++]=input[i+1];i++;}
        }
        else {
            arg[num++]=input[i];
        }
    }
    if (num==0)return;
    arg[num]=NULL;
    int stored[1000];
    for (int i=0;i<file_count;i++){
        stored[i]=open(files[i],O_RDONLY);
        if (stored[i]<0){
            fprintf(stderr,"cshell: no such file or directory\n");
            for (int j=0;j<i;j++)close(stored[j]);
            return;}
    }

    char* cmd=arg[0];
    char* p=path(cmd);
    if (p==NULL){
        if (cmd[0]=='%'){
            fprintf(stderr,"cshell: command not found (%s)\n",cmd+1);

        }
        else {fprintf(stderr,"cshell: command not found (%s)\n",cmd);}
        for (int i=0;i<file_count;i++)close(stored[i]);
        return;
    }
    if (arg[0][0]=='%'){arg[0]+=1;}
int pipefd[2] = {-1, -1};
    pid_t feeder_pid = -1;
    if (file_count > 1) {
        if (pipe(pipefd) < 0) {
            perror("pipe error");
        }
        feeder_pid = fork();
        if (feeder_pid == 0) {
            close(pipefd[0]); 
            char buf[4096];
            ssize_t bytes_read;
            for (int i = 0; i < file_count; i++) {
                while ((bytes_read = read(stored[i], buf, sizeof(buf))) > 0) {
                    write(pipefd[1], buf, bytes_read);
                }
                close(stored[i]);
            }
            close(pipefd[1]);
            exit(EXIT_SUCCESS);
        }
    }
pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
    } 
    else if (pid == 0) {
        if (file_count == 1) {
            dup2(stored[0], STDIN_FILENO);
        } else if (file_count > 1) {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
        }
        for (int i = 0; i < file_count; i++) close(stored[i]);
        execv(p, arg);
        perror("execv error");
        exit(EXIT_FAILURE);
    } 
    else {
        if (file_count > 1) {
            close(pipefd[0]);
            close(pipefd[1]);
        }
        
        for (int i = 0; i < file_count; i++) close(stored[i]);
        waitpid(pid, NULL, 0);
        if (feeder_pid > 0) {
            waitpid(feeder_pid, NULL, 0);
        }
    }
    free(p);
   
}