#include"hop.h"
#include<stdio.h>
#include"shell_prompt.h"
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <linux/limits.h>
#include<time.h>
typedef struct{
    char path[PATH_MAX];
    int num,time;
    double score;
}hops;

static char prev[PATH_MAX]="";
static int compare(const void*a,const void*b){
    hops *h1=(hops*)a;
    hops *h2=(hops*)b;if (h1->score<h2->score)return 1;
    if (h1->score==h2->score)return 0;
    return -1;
}

static void history(char *buffer) {
    const char *home = getenv("HOME");
    if (home!=NULL) {
    snprintf(buffer, PATH_MAX, "%s/.shell_history", home);
    }
}
static void new_rec(char*destpath){
    char h[PATH_MAX];
    history(h);
hops data[1000];
int s=0;
FILE *f=fopen(h,"r");
if (f){
    char line[PATH_MAX];
    while(fgets(line,PATH_MAX,f)){
    char*freq=strtok(line," \t");
    char*time=strtok(NULL," \t");
    if (freq&&time){
        data[s].num=atoi(freq);
        data[s].time=atoi(time);
        char*path=strtok(NULL,"\r\n");
        if (path){
            while(*path==' '||*path=='\t'){path++;}
        }
        strncpy(data[s].path,path,sizeof(data[s].path)-1);
        data[s].path[sizeof(data[s].path)-1]='\0';
        s++;
    }   
    }
    fclose(f) ;
}
int found=0;
for(int i=0;i<s;i++){
    if(strcmp(data[i].path,destpath)==0){data[i].num++;data[i].time=time(NULL);found=1;break;}
}
if(!found){
    strcpy(data[s].path,destpath);
    data[s].num=1;
    data[s].time=time(NULL);
    s++;
}
f=fopen(h,"w");
if(f){
    for(int i=0;i<s;i++){
        fprintf(f,"%d %d %s\n",data[i].num,data[i].time,data[i].path);
    }
    fclose(f);
}
}


static int frecency(char*dest){
    char h[PATH_MAX];
    history(h);
    hops match[1000];
    int m_count=0;
    FILE*f =fopen(h,"r");
    if (!f)return -1;
    char line[PATH_MAX];
    int curr_time=time(NULL);
while (fgets(line, sizeof(line), f)) {
        hops entry;
        if (sscanf(line, "%d %d %[^\n]", &entry.num, &entry.time, entry.path) == 3) {
            if (strstr(entry.path, dest) != NULL) {
                double age_seconds = difftime(curr_time, entry.time);
                double max_age = 90.0 * 24.0 * 3600.0;
                double factor = 1.0 - (age_seconds / max_age);
                if (factor < 0.1) {
                    factor = 0.1;
                }
                entry.score = entry.num * factor;
                match[m_count++] = entry;
            }
        }
   } fclose(f);
    if (m_count==0){
        return -1;
    }
    qsort(match,m_count,sizeof(hops),compare);
    for (int i=0;i<m_count;i++){if (chdir(match[i].path)==0){return 0;}}
return -1;
    }



static void hopto(char*input){
    char curr[PATH_MAX];
    char dest[PATH_MAX];
    if (getcwd(curr,PATH_MAX)==NULL){perror("getcwd");}
   else if (strcmp(input,".")==0){strcpy(prev, curr);}
    else if (strcmp(input,"-")==0){
        if (strlen(prev)==0){fprintf(stderr,"no previous directory\n");return;}
        if (chdir(prev)==0){
            strcpy(prev,curr);
            char next[PATH_MAX];
            if (getcwd(next,PATH_MAX)==NULL){perror("hop");}
            else {new_rec(next);}
        }
    }
    else {if (strcmp(input,"~")==0){
       
        strcpy(dest,home_shell);
    }
    else if (strncmp(input,"~/", 2)==0) {
        strcpy(dest,home_shell);strcat(dest,"/");strcat(dest,input+2);}
    else strcpy(dest,input);
if (chdir(dest)==0){
    strcpy(prev,curr);
    char next[PATH_MAX];
    if (getcwd(next,PATH_MAX)!=0){
new_rec(next);
    }
return;
}
if (frecency(input)==0){
    strcpy(prev,curr);
    char next[PATH_MAX];
    if (getcwd(next,PATH_MAX)){new_rec(next);}
}
else {fprintf(stderr,"hop: no such directory\n");}


}
}


void hopper(char**input,int n){
    if (n==0){hopto("~");
    return;}
    for (int i=0;i<n;i++){
        hopto(input[i]);
    }
}