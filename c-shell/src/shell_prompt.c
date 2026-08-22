#include "shell_prompt.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <linux/limits.h>
char home_shell[PATH_MAX];
static size_t home_len;
void home(){
    if (!getcwd(home_shell, sizeof(home_shell))){
        perror("error: cannot start directory");exit(1);
    }
    home_len=strlen(home_shell);


}

void prompt(){
    char host[256];
    char cwd[PATH_MAX];
 char *username;
    struct passwd *pw=getpwuid(getuid());
    if (pw){username=pw->pw_name;}
    if (username==NULL){username="unknown";}
    if (gethostname(host,sizeof(host))!=0) {
        strncpy(host, "unknown", sizeof(host));
    }
    if (getcwd(cwd,sizeof(cwd))==NULL){
        perror("getcwd failed");
        return;
    }
    if (!strncmp(cwd, home_shell,home_len)){
        if (cwd[home_len]=='\0'||cwd[home_len]=='/'){
            printf("<%s@%s:~%s> ",username,host,cwd+home_len);
            fflush(stdout);
            return;
        }
    }
    printf("<%s@%s:%s> ",username,host,cwd);
    fflush(stdout);
}