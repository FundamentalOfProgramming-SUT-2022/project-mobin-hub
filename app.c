#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include<unistd.h>
#include <sys/stat.h>
#include<unistd.h>
#include <math.h>
#include <ctype.h>

#define  N 200

int depth;

char *clipboard;

char *str, *str1, *str2;
char **paths, **history, **prv_sit;
char *path;
int lft, rit, at, backward, forward, siz, is_wild;
int *arr;
int arr_sz, file_cnt, hist_cnt;

bool count, all, byword, pr, end_of_file, Cflag, Lflag;
char *file_to_string(FILE *);
char *concat(char*, char*);


char *ntab(int n){
    printf("%d\n", n);
    if(n <= 0)return "";
    char * res = "\t";
    while(--n)res = concat(res, "\t");
    return res;
}

int backup_ind(char *path){
    int ind = -1;
    for(int i=0;i<hist_cnt;i++){
        if(strcmp(history[i], path) == 0){
            ind = i;
            break;
        }
    }
    return ind;
}

int get_ind(char *path){
    int ind = backup_ind(path);
    if(ind == -1){
        ind = hist_cnt;
    }
    if(ind == hist_cnt){
        history[hist_cnt++] = path;
    }
    return ind;
}

void get_backup(char *path){
    int ind = get_ind(path);
    FILE *file = fopen(path, "r");
    prv_sit[ind] = file_to_string(file);
    fclose(file);
}

void undo(){
    int ind = backup_ind(path);
    if(ind == -1){
        printf("invalid command\n");
        return;
    }
    FILE *file = fopen(path, "r");
    char *tmp = file_to_string(file);
    fclose(file);
    file = fopen(path, "w");
    fputs(prv_sit[ind], file);
    fclose(file);
    prv_sit[ind] = tmp;
}

void trim(char *input){
    *(input + strlen(input) - 1) = '\0';
}

char *file_to_string(FILE *file){
    char *res = malloc(N*N*sizeof(char));
    char x = getc(file);
    int ind = 0;
    while(x != EOF){
        *(res + ind) = x;
        ind++;
        x = getc(file);
    }
    *(res + ind) = '\0';
    return res;
}

char *get_til(char *a ,char c){
    char *res = malloc(N*sizeof(char));
    *(res + strlen(a)) = '\0';
    for(int i=0;i<strlen(a);i++){
        char x = a[i];
        if(x != c){
            *(res + i) = x;
        }else{
            *(res + i) = '\0';
            break;
        }
    }
    return res;
}

char *concat(char *a, char *b){
    char *res = malloc(N*sizeof(char));
    strcpy(res, a);
    strcpy(res+strlen(a), b);
    return res;
}

char *get_word(char *in){
    char *res = malloc(N*sizeof(char));
    *(res + strlen(in)) = '\0';
    for(int i=0;i<strlen(in);i++){
        char x = in[i];
        if(x != ' ' && x != EOF && x != '\n'){
            *(res + i) = x;
        }else{
            *(res + i) = '\0';
            break;
        }
    }
    return res;
}

void get_flag(char *flag){
    char *part = get_word(flag);
    flag += strlen(part)+1;
    if(strcmp(part, "str") == 0){
        if(flag[0] == '\"'){
            str = get_til(flag+1, '\"');
        }else{
            str = flag;
        }
    }
    if(strcmp(part, "str1") == 0){
        if(flag[0] == '\"'){
            str1 = get_til(flag+1, '\"');
        }else{
            str1 = flag;
        }
    }
    if(strcmp(part, "str2") == 0){
        if(flag[0] == '\"'){
            str2 = get_til(flag+1, '\"');
        }else{
            str2 = flag;
        }
    }
    if(strcmp(part, "file") == 0){
        if(flag[0] == '\"'){
            path = get_til(flag+1, '\"');
        }else{
            path = flag;
        }
    }
    if(strcmp(part, "files") == 0){
        file_cnt = 0;
        while(strlen(flag) > 0){
            // printf("%s\n", flag);
            if(flag[0] == '\"'){
                paths[file_cnt] = get_til(flag+1, '\"');
                flag += strlen(paths[file_cnt])+3;
            }else{
                paths[file_cnt] = get_til(flag, ' ');
                flag += strlen(paths[file_cnt])+1;
            }
            // printf("%s\n", paths[file_cnt]);
            file_cnt++;
        }
    }
    if(strcmp(part, "pos") == 0){
        char *tmp; 
        tmp = get_til(flag, ':');
        lft = atoi(tmp);
        flag += strlen(tmp)+1;
        rit = atoi(get_word(flag));

    }
    if(strcmp(part, "at") == 0){
        at = atoi(get_word(flag));
    }
    if(strcmp(part, "all") == 0){
        all = 1;
    }
    if(strcmp(part, "count") == 0){
        count = 1;
    }
    if(strcmp(part, "byword") == 0){
        byword = 1;
    }
    if(strcmp(part, "b") == 0){
        backward = 1;
    }else{
        forward = 1;
    }
    if(strcmp(part, "size") == 0){
        siz = atoi(get_word(flag));
    }
    if(strcmp(part, "c") == 0){
        Cflag = true;
    }
    if(strcmp(part, "l") == 0){
        Lflag = true;
    }
}

void get_flags(char *flags){
    printf("initializing flags...\n");
    printf("(*%s*)", flags);
    while(strlen(flags)>0){
        char *flag = get_til(flags+1, '-');
        flags += strlen(flag)+1;
        if(flag[strlen(flag)-1] == ' '){
            trim(flag);
        }
        printf("(*%s*)", flag);
        get_flag(flag);
    }
    printf("flags are collected\n");
}

void print_flags(){
    printf("flags are as follows :\n");
    printf("path is : %s\n", path);
    printf("str is : %s\n", str);
    printf("pos is : (%d, %d)\n", lft, rit);
    printf("at (%d)\n", at);
    printf("count is %d\n", count);
    printf("byword is %d\n", byword);
    printf("all is %d\n", all);
}

char *getln(){
    char * x = malloc(N * sizeof(char));
    int ind = 0;
    char c;
    c = getchar();
    while(c != '\n'){
        *(x+ind) = c;
        ind++;
        c = getchar();
    }
    *(x+ind) = '\0';
    return x;
}

char *getln_file(FILE* file){
    char *res = malloc(N*N*sizeof(char));
    char x = getc(file);
    // printf("%c in getln\n", x);
    int ind = 0;
    while(x != '\n' && x != EOF){
        *(res + ind) = x;
        x = getc(file);
        ind++;
    }
    end_of_file = (x == EOF);
    *(res + ind) = '\0';
    return res;
}

int exists(const char *fname){
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
        return 1;
    }
    return 0;
}

int find_pos(char *path, int row, int col){
    if(path[0] == '/')path++;
    FILE *file = fopen(path, "r");
    int res = 0;
    char x;
    while(row>1){
        x = getc(file);
        res++;
        if(x == EOF){
            return -1;
        }
        if(x == '\n')row--;
    }
    return res+col;
    fclose(file);
}

void create_file(char *path){
    if(*(path+strlen(path)-1) == '\"'){
        trim(path);
        path++;
    }
    if(path[0] == '/')path++;
    char *token = strtok(path, "/");
    if(strcmp(token, "root") != 0){
        printf("invalid command\n");
    }
    while(true){
        char *st = strtok(NULL, "/");
        if(st == NULL){
            if(exists(token)){
                printf("invalid command\n");
                goto hell;
            }
            FILE *file = fopen(token, "w");
            fclose(file);
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
}

void insert(char *path, char *str, int l){
    FILE *file = fopen(path, "r");
    char *last = file_to_string(file);
    char *res = malloc(N*N*sizeof(char));
    strcpy(res, last);
    *(res + l) = '\0';
    res = concat(res, str);
    res = concat(res, last+l);
    fclose(file);
    FILE *nfile = fopen(path, "w");
    fputs(res, nfile);
    fclose(nfile);
}

void insertstr(char *path, char *str, int row, int col){
    col++;
    get_backup(path);
    if(path[0] == '/')path++;
    int ind=0;
    char *newfile = malloc(N*N*sizeof(char));
    FILE *file = fopen(path, "r");
    char x;
    while(row>1 && x != EOF){
        x = getc(file);
        *(newfile+(ind++)) = x;
        if(x == EOF){
            fclose(file);
            printf("invalid command\n");
            return;
        }
        if(x == '\n'){
            row--;
        }
    }
    while(col>1){
        col--;
        x = getc(file);
        *(newfile+(ind++)) = x;
        // newfile++;
        if(x == '\n'){
            fclose(file);
            printf("invalid command\n");
            return;
        }
    }
    strcpy(newfile+ind, str);
    ind += strlen(str);
    while(true){
        x = getc(file);
        if(x == EOF)break;
        *(newfile+(ind++)) = x;
    }
    fclose(file);
    FILE *nfile = fopen(path, "w");
    fputs(newfile, nfile);
    fclose(nfile);
    return;
}

void simple_remove(char *path, int l, int r){
    int sz = r-l+1;
    int ind=0;
    char *newfile = malloc(N*N*sizeof(char));
    FILE *file = fopen(path, "r");
    char x;
    x = getc(file);
    for(;ind<l && x != EOF;ind++){
        *(newfile + ind) = x;
        x = getc(file);
    }
    for(int i=0;i<sz && x != EOF && ind+i<=r;i++){
        x = getc(file);
    }
    for(;x != EOF;ind++){
        *(newfile + ind) = x;
        x = getc(file);
    }
    *(newfile + ind) = '\0';
    fclose(file);
    FILE *nfile = fopen(path, "w");
    fputs(newfile, nfile);
    fclose(nfile);
}

void removestr(char *path, int row, int col, int sz){
    get_backup(path);
    if(path[0] == '/')path++;
    int l,r;
    if(forward){
        l = find_pos(path, row, col);
        r = l+sz-1;
    }else{
        r = find_pos(path, row, col);
        l = r-sz+1;
    }
    if(l == -1 || r == -1){
        printf("invalid command\n");
        return;
    }
    simple_remove(path, l, r);
}

void print_file(char *path){
    if(path[0] == '/')path++;
    FILE *file = fopen(path, "r");
    char x;
    x = getc(file);
    while(x != EOF){
        printf("%c", x);
        x = getc(file);
    }
    printf("\n");
    fclose(file);
    return;
}

void copystr(char *path, int row, int col, int sz){
    if(path[0] == '/')path++;
    int l,r;
    if(forward){
        l = find_pos(path, row, col);
        r = l+sz-1;
    }else{
        r = find_pos(path, row, col);
        l = r-sz+1;
    }
    if(l == -1 || r == -1){
        printf("invalid command\n");
        return;
    }
    int ind=0;
    FILE *file = fopen(path, "r");
    char x;
    x = getc(file);
    for(;ind<l && x != EOF;ind++){
        x = getc(file);
    }
    for(int i=0;i<sz && x != EOF && ind+i<=r;i++){
        *(clipboard + i) = x;
        *(clipboard + i+1) = '\0';
        x = getc(file);
    }
    fclose(file);
}

void cutstr(char *path, int row, int col, int sz){
    print_flags();
    if(path[0] == '/')path++;
    copystr(path, row, col, sz);
    print_file(path);
    removestr(path, row, col, sz);
    print_file(path);
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

int find_from(int stt, char *ptrn){
    FILE *file = fopen(path, "r");
    char *string = file_to_string(file);
    string += stt;
    int tmp = find_in_str(string, ptrn);
    if(tmp == -1){
        return -1;
    }
    fclose(file);
    return stt + tmp;
}

int expand(char *string, int ind, int lim){
    if(is_wild == 0){
        return ind;
    }
    while(ind+is_wild>=lim && string[ind+is_wild] != ' ' && string[ind+is_wild] != '\n' && string[ind+is_wild] != EOF){
        ind += is_wild;
    }
    return ind;
}

int count_in_string(int stt, char *string){
    int ind = stt-1;
    int res = 0;
    // printf("find_from(%d, %s) = %d\n", ind+1, str, find_from(ind+1, str));
    while(find_from(ind+1, str) != -1){
        ind = find_from(ind+1, str);
        if(is_wild == 0){
            res++;
            continue;
        }
        if(is_wild == -1){
            res += ind-expand(string,ind,stt)+1;
        }else{
            res += expand(string, ind+strlen(str)-1, stt)-(ind+strlen(str)-1)+1;
        }
    }
    return res;
}

int if_byword(char *string, int x){
    if(!byword){
        return x;
    }
    int res = 0;
    int ind = 0;
    while(ind<x){
        while(ind<strlen(string) && (string[ind] == ' ' || string[ind] == '\n')){
            ind++;
        }
        res++;
        char *tmp = get_word(string+ind);
        ind += strlen(tmp);
    }
    return res;
}

int find(){
    // print_flags();
    FILE *file = fopen(path, "r");
    char *string = file_to_string(file);
    fclose(file);
    if(str[0] == '*'){
        str++;
        is_wild = -1;
    }
    if(str[strlen(str)-1] == '*' && str[strlen(str)-2] != '\\'){
        trim(str);
        is_wild = 1;
    }
    if(!count && !at && !all){
        at = 1;
    }
    if(count){
        if(pr)printf("%d\n", count_in_string(0, string));
        return 0;
    }
    if(at){
        int rmn = at;
        int total = count_in_string(0, string);
        if(total<rmn){
            if(pr)printf("-1\n");
            return -1;
        }
        for(int i=0;i+1<strlen(string);i++){
            int tmp = count_in_string(i, string)-count_in_string(i+1, string);
            if(tmp >= rmn){
                if(pr)printf("%d\n", if_byword(string, i));
                return i;
            }else{
                rmn -= tmp;
            }
        }
        if(pr)printf("%d\n", if_byword(string, strlen(string)-1));
        return strlen(string)-1;
    }
    if(all){
        int last = 0;
        arr_sz = 0;
        for(int i=strlen(string)-1;i>=0;i--){
            if(count_in_string(i, string) > last){
                last = count_in_string(i, string);
                if(pr)printf("%d,", if_byword(string, i));
                arr[arr_sz++] = i;
            }
        }
        if(pr)printf("\n");
        return 0;
    }
}

void _replace_at(){
    char *mystr = malloc(N*N*sizeof(char));
    strcpy(mystr, str);
    if(mystr[0] == '*'){
        int l = find();
        mystr++;
        int r = find() + strlen(mystr) - 1;
        simple_remove(path, l, r);
        insert(path, str2, l);
    }else{
        if(mystr[strlen(mystr)-1] == '*' && mystr[strlen(mystr)-2] != '\\'){
            trim(mystr);
        }
        int l = find();
        int r = l+strlen(mystr)-1;
        simple_remove(path, l, r);
        insert(path, str2, l);
    }
    return;
}

void replace(){
    pr = false;
    if(at && all){
        printf("invalid command\n");
        return;
    }
    str = str1;
    if(at){
        if(find() == -1){
            printf("invalid command\n");
            return;
        }
        _replace_at();
        return;
    }
    if(all){
        at = 1;
        int ind = find();
        while(ind != -1){
            _replace_at();
            ind = find();
        }
        return;
    }
    printf("invalid command\n");
}

void grep(){
    // printf("%d\n", file_cnt);
    // for(int i=0;i<file_cnt;i++){
    //     printf("%s\n", paths[i]);
    // }
    if(Cflag && Lflag){
        printf("invalid command\n");
    }
    int res = 0;
    for(int i=0;i<file_cnt;i++){
        bool has = false;
        FILE *file = fopen(paths[i], "r");
        // printf("file is open\n");
        end_of_file = 0;
        while(!end_of_file){
            char *line = getln_file(file);
            // printf("%s\n", line);
            if(find_in_str(line, str) != -1){
                if(Cflag){
                    res++;
                    continue;
                }
                if(Lflag){
                    has = true;
                    continue;
                }
                printf("%s: %s\n", paths[i], line);
            }
        }
        if(has){
            printf("%s\n", paths[i]);
        }
        fclose(file);
    }
    if(Cflag){
        printf("%d\n", res);
    }
}

void auto_indent(){
    pr = false;
    str = "{";
    at = 1;
    while(find() != -1){
        FILE *file = fopen(path, "r");
        char *string = file_to_string(file);
        fclose(file);
        int ind = find();
        if(string[ind+1] != '\n'){
            insert(path, "\n", ind+1);
        }
        at++;
    }
    str = "}";
    at = 1;
    while(find() != -1){
        FILE *file = fopen(path, "r");
        char *string = file_to_string(file);
        fclose(file);
        int ind = find();
        for(int i=ind-1;i>=0;i--){
            if(string[i] == '\n')break;
            if(!isspace(string[i])){
                insert(path, "\n", ind);
                break;
            }
        }
        at++;
    }
    int tab_cnt = 0;
    FILE *file = fopen(path, "r");
    char *newfile = "";
    while(true){
        char *line = getln_file(file);
        if(end_of_file){
            break;
        }
        if(line[strlen(line)-1] == '}'){
            tab_cnt--;
        }
        //printf("(%s)\n", concat(ntab(tab_cnt), line));
        while(strlen(line)>0 && (line[0] == '\t' || line[0] == ' '))line++;
        char * pref = ntab(tab_cnt);
        // line = concat(ntab(tab_cnt), line);
        if(line[strlen(line)-1] == '{'){
            trim(line);
            while(line[strlen(line) - 1] == ' ' || line[strlen(line) - 1] == '\t')trim(line);
            if(strlen(line)){
                line = concat(line, " {");
            }else{
                line = concat(line, "{");
            }
            tab_cnt++;
        }
        line = concat(pref, line);
        line = concat(line, "\n");
        newfile = concat(newfile, line);
    }
    fclose(file);
    file = fopen(path, "w");
    fputs(newfile, file);
    fclose(file);
}

int main(){
    paths = malloc(N*N*sizeof(char*));
    prv_sit = malloc(N*N*sizeof(char*));
    history = malloc(N*N*sizeof(char*));
    arr = malloc(N*N*sizeof(int));
    clipboard = (char *)malloc(N*N*sizeof(char));
    path = "root/dir1/file.txt"; // for debugging purpose
    while(true){
        at = lft = rit = 0;
        all = byword = backward = count = Cflag = Lflag =false;
        pr = true;
        str = "";
        char *line = getln();
        // printf("((%s))", line);
        char *command = get_til(line, ' ');
        // printf("(%s)\n", command);
        line += strlen(command)+1;
        get_flags(line);
        // print_flags();
        if(strcmp(command, "createfile") == 0){
            create_file(path);
            continue;
        }
        if(strcmp(command, "insertstr") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            print_flags();
            insertstr(path, str, lft, rit);
            continue;
        }
        if(strcmp(command, "removestr") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            removestr(path, lft, rit, siz);
            continue;
        }
        if(strcmp(command, "cat") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            print_file(path);
            continue;
        }
        if(strcmp(command, "copystr") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            copystr(path, lft, rit, siz);
            continue;
        }
        if(strcmp(command, "cutstr") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;;
            }
            cutstr(path, lft, rit, siz);
            continue;
        }
        if(strcmp(command, "pastestr") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            insertstr(path, clipboard, lft, rit);
            continue;
        }
        if(strcmp(command, "find") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            find();
            continue;
        }
        if(strcmp(command, "replace") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            replace();
            continue;
        }
        if(strcmp(command, "grep") == 0){
            for(int i=0;i<file_cnt;i++){
                if(!exists(paths[i])){
                    printf("invalid command\n");
                    continue;
                }
            }
            grep();
            continue;
        }
        if(strcmp(command, "undo") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            undo();
            continue;
        }
        if(strcmp(command, "auto-indent") == 0){
            if(!exists(path)){
                printf("invalid command\n");
                continue;
            }
            auto_indent();
            continue;
        }
        if(strcmp(command, "compare") == 0){
            
        }
        break;
    }
    return 0;
}