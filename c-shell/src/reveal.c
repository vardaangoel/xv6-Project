#include "reveal.h"
#include "shell_prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<pwd.h>
#include<grp.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include<time.h>
static void print_info(char* name,struct stat *sb,int l){
    if (!l){printf("%s\n",name);return;}
    printf((S_ISDIR(sb->st_mode)) ? "d" : (S_ISLNK(sb->st_mode)) ? "l" : "-");
    printf((sb->st_mode & S_IRUSR) ? "r" : "-");
    printf((sb->st_mode & S_IWUSR) ? "w" : "-");
    printf((sb->st_mode & S_IXUSR) ? "x" : "-");
    printf((sb->st_mode & S_IRGRP) ? "r" : "-");
    printf((sb->st_mode & S_IWGRP) ? "w" : "-");
    printf((sb->st_mode & S_IXGRP) ? "x" : "-");
    printf((sb->st_mode & S_IROTH) ? "r" : "-");
    printf((sb->st_mode & S_IWOTH) ? "w" : "-");
    printf((sb->st_mode & S_IXOTH) ? "x" : "-");
    struct passwd *pw = getpwuid(sb->st_uid);
    struct group  *gr = getgrgid(sb->st_gid);
    char time_buf[256];
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&sb->st_mtime));
    printf(" %2lu %-8s %-8s %8ld %s %s\n",
           (unsigned long)sb->st_nlink,
           pw ? pw->pw_name : "unknown",
           gr ? gr->gr_name : "unknown",
           (long)sb->st_size,
           time_buf,
           name);
}
static int compare(const void *a, const void *b){
    return strcmp(*(char**)a,*(char**)b);
}
static void reveal_direc(char*path,int a,int l){
char p[PATH_MAX];
if (strcmp(path,"~")==0){strcpy(p,home_shell);}
else if (strncmp(path,"~/",2)==0){
    strcpy(p,home_shell);
    strcat(p,"/");
    strcat(p,path+2);
}    else {strcpy(p,path);}
struct stat sb;
    if (lstat(p, &sb) == -1) {
        perror("Error reading path");
        return;
    }
    if (S_ISDIR(sb.st_mode)) {
        DIR *dir = opendir(p);
        if (!dir) {
            perror("Error opening directory");
            return;
        }
        struct dirent *entry;
        char*all[10000];int count=0;
        while ((entry = readdir(dir)) != NULL&&count<10000) {
            if (!a&&entry->d_name[0]== '.') {
                continue;
            }
            all[count++]=strdup(entry->d_name);}closedir(dir);
            qsort(all,count,sizeof(char*),compare);
            for (int i=0;i<count;i++){

            
        char full_path[PATH_MAX+2];
        snprintf(full_path, sizeof(full_path), "%s/%s", p, all[i]);
        struct stat entry_stat;
            if (lstat(full_path, &entry_stat) == -1) {free(all[i]);
                continue;
            }
            print_info(all[i], &entry_stat, l);
            free(all[i]);
        }
        
    } else {
        print_info(path, &sb, l);
    }
}



void reveal(char**input,int count){
    int a=0,l=0;
    char*path[256];
int path_cnt=0;
for (int i=0;i<count;i++){
    if (input[i][0]=='-'){
        for (int j=1;input[i][j]!='\0';j++){
            if (input[i][j]=='a'){
                a=1;
            }
            else if(input[i][j]=='l'){
                l=1;
            }
            else if (input[i][j]=='\n'||input[i][j]=='\r'||input[i][j]==' '){continue;}
            else {
                fprintf(stderr,"invalid flag\n");
                return;
            }
        }
    }
    else{path[path_cnt]=input[i];path_cnt++;}
}
if (path_cnt==0){path[0]=".";path_cnt=1;}
for (int i=0;i<path_cnt;i++){
    if (path_cnt>1) {
            printf("%s:\n",path[i]);
        }
    reveal_direc(path[i],a,l);     
        if (i<path_cnt-1) printf("\n");
    }
}


