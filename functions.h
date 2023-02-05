
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#define print(x) printf("%d \n", x);
#define N 10005

char *clipboard;

void trim(char *input){
    *(input + strlen(input) - 1) = '\0';
}

char *file_to_string(FILE *file){
    char *res = malloc(N*sizeof(char));
    char x = getc(file);
    int ind = 0;
    while(x != EOF){
        if(x == '\t'){
            for(int i=0;i<4;i++){
                *(res + ind) = ' ';
                ind++;
            }
            x = getc(file);
            continue;
        }
        *(res + ind) = x;
        ind++;
        x = getc(file);
    }
    *(res + ind) = '\0';
    return res;
}

void file_to_given_string(char* string, FILE *file){
    strcpy(string, file_to_string(file));
}


void write_to_file(char* string, char* path){
   if(*path == '/')path++;
   FILE *file;
   file = fopen(path, "w");
   fputs(string, file);
   fclose (file);
   return;
}

bool exists(const char *fname){
    if(fname[0] == '/')fname++;
    // printf("\n(%s)\n", fname);
    char *root = "root/";
    for(int i=0;i<5;i++){
        if(root[i] != fname[i]){
            return 0;
        }
    }
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}

bool create_file(char *path){
    bool res;
    int depth = 0;
    if(*(path+strlen(path)-1) == '\"'){
        trim(path);
        path++;
    }
    if(path[0] == '/')path++;
    char *token = strtok(path, "/");
    if(strcmp(token, "root") != 0){
        res = false;
        goto hell;
    }
    while(true){
        char *st = strtok(NULL, "/");
        if(st == NULL){
            if(exists(token)){
                res = false;
                goto hell;
            }
            FILE *file = fopen(token, "w");
            fclose(file);
            res = true;
            break;
        }else{
            if(chdir(token) != 0){
                mkdir(token, S_IRWXU);
                chdir(token);
            }
            depth++;
            token = st;
        }
    }
    hell:
    while(depth>0){
        depth--;
        chdir("../");
    }
    return res;
}

int find_in_str(char *string, char *ptrn){
    for(int i=0;i+strlen(ptrn)-1<strlen(string);i++){
        for(int j=0;j<strlen(ptrn);j++){
            if(string[i+j] != ptrn[j]){
                break;
            }
            if(j+1 == strlen(ptrn)){
                return i;
            }
        }
    }
    return -1;
}

bool is_prefix(char* st1, char* st2){
    return (find_in_str(st1, st2) == 0);
}

void handle_space(char* string){
    char res[N] = {"\0"};
    int ind = 0;
    for(int i = 0; i < strlen(string); i++){
        if(string[i] == '\t'){
            for(int j=0;j<4;j++){
                res[ind++] = ' ';
            }
            continue;
        }
        res[ind++] = string[i];
    }
    strcpy(string, res);
}

void auto_indent(char *st1){
    char *st2 = malloc(N*sizeof(char));
    memset(st2, '\0', sizeof(st2));
    int cnt_tab = 0;
    for(int i=0;i < strlen(st1);i++){
        while(isspace(st1[i]))i++;
        if(st1[i] == '\n')continue;
        for(int t = 0; t < cnt_tab; t++)st2[strlen(st2)] = '\t';

        bool fl_char = 0;
        for(;i < strlen(st1);i++){
            if(st1[i] == '\n')break;
            if(st1[i] == '}'){
                st2[strlen(st2)] = '\n';
                cnt_tab--;
                for(int t = 0; t < cnt_tab; t++)
                    st2[strlen(st2)] = '\t';

                st2[strlen(st2)] = '}';
                st2[strlen(st2)] = '\n';
                break;
            }
            if(st1[i] == '{'){
                while(strlen(st2) && fl_char && isspace(st2[strlen(st2)-1]))st2[strlen(st2)-1] = '\0';
                if(fl_char)st2[strlen(st2)] = ' ';   
                st2[strlen(st2)] = '{';
                st2[strlen(st2)] = '\n';   
                cnt_tab++;    
                break;
            }
            fl_char = fl_char || !isspace(st1[i]);
            st2[strlen(st2)] = st1[i];
        }
    }
    strcpy(st1, st2);
    handle_space(st1);
}


#endif
