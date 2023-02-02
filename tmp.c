#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<unistd.h>
#include<ncurses.h>
#include<stdbool.h>
// #include<"functions.h">

#define N 10005

WINDOW *command_win, *status_win, *name_win;

struct Editor{
    char mode;
    int x, y;
    int line;
    bool save;
    char* file_name; 
    char Buffer[N][N];
    char Bar[N];
}ed;

void curses_init(){
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
    // start_color();
    // init_pair(1, COLOR_RED, COLOR_BLUE);
    // init_pair(2, COLOR_WHITE, COLOR_MAGENTA);
    // init_pair(3, COLOR_RED, COLOR_BLUE);
}

void mymove(int y, int x){
    move(y, 2);
    for(int i=0;i<x;i++){
        printw("%c", ed.Buffer[ed.y][i]);
    }
}


int length(int y, int til){
    move(y, 2);
    for(int i=0;i<til;i++){
        printw("%c", ed.Buffer[y][i]);
    }
    int res;
    getyx(stdscr, y, res);
    sprintf(ed.Bar, "((%d %d))", y, res);
    return res;
}

int find_x(int y, int xx){
    int res = 0;
    while(res+1<strlen(ed.Buffer[y]) && length(y, res+1) <= xx)res++;
    return res;
}

char *concat(char *a, char *b){
    char *res = malloc(N*sizeof(char));
    strcpy(res, a);
    strcpy(res+strlen(a), b);
    return res;
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

void file_to_given_string(char* string, FILE *file){
    strcpy(string, file_to_string(file));
}

bool navigate_arrow(int ch){
    if(ch == KEY_RIGHT){
        if(ed.x+1 == strlen(ed.Buffer[ed.y])){
            return true;
        }else{
            ed.x++;
        }
        return true;
    }
    if(ch == KEY_LEFT){
        if(ed.x == 0){
            return true;
        }else{      
            ed.x--;
        }
        return true;
    }
    if(ch == KEY_UP){
        if(ed.y == 0){
            ed.x = 0;
            return true;
        }
        ed.y--;
        ed.x = find_x(ed.y, length(ed.y+1, ed.x));
        return true;
    }
    if(ch == KEY_DOWN){
        if(ed.y+1 == ed.line){
            ed.x = strlen(ed.Buffer[ed.y])-1;
            return true;
        }
        ed.y++;
        ed.x = find_x(ed.y, length(ed.y-1, ed.x));
        return true;
    }
    return false;
}

bool navigate_hjkl(int ch){
    if(ch == 'l'){
        if(ed.x+1 == strlen(ed.Buffer[ed.y])){
            return true;
        }else{
            ed.x++;
        }
        return true;
    }
    if(ch == 'h'){
        if(ed.x == 0){
            return true;
        }else{      
            ed.x--;
        }
        return true;
    }
    if(ch == 'k'){
        if(ed.y == 0){
            ed.x = 0;
            return true;
        }
        ed.y--;
        ed.x = find_x(ed.y, length(ed.y+1, ed.x));
        return true;
    }
    if(ch == 'j'){
        if(ed.y+1 == ed.line){
            ed.x = strlen(ed.Buffer[ed.y])-1;
            return true;
        }
        ed.y++;
        ed.x = find_x(ed.y, length(ed.y-1, ed.x));
        return true;
    }
    return false;
}

void perform(char *command){
    command++;
    if(strcmp(command, "save") == 0){

    }
}

void init(char *file_n){
    ed.file_name = file_n;
    ed.mode = 'N';
    ed.save = true;
    ed.x = 0, ed. y = ed.line = 0;
    ed.Bar[0] = '\0';
    if(*file_n == '/')file_n++;
    printf("%s\n", file_n);
    FILE *file;
    file = fopen(file_n, "r");
    char st[N] = {'\0'};
    file_to_given_string(st, file);
    for(int i=0;i<strlen(st);i++){
        int j = 0;
        while(i<strlen(st) && st[i] != '\n'){
            ed.Buffer[ed.line][j++] = st[i];
            i++;
        }
        ed.line++;
    }
    fclose(file);
}



void printBuff(){
    for(int i = 0; i < ed.line; i++){
        mvprintw(i, 0, "%d %s\n", i, ed.Buffer[i]);
    }
    int height, width;
    getmaxyx(stdscr, height, width);
    refresh();
    if(ed.mode == 'N' || ed.mode == 'C'){
        mvwprintw(status_win, 0, 0, " NORMAL ");
    }
    if(ed.mode == 'V'){
        mvwprintw(status_win, 0, 0, " VISUAL ");
    }
    if(ed.mode == 'I'){
        mvwprintw(status_win, 0, 0, " INSERT ");
    }
    mvwprintw(name_win, 0, 0, " %s",  ed.file_name);
    if(!ed.save){
        wprintw(status_win, " +");
    }
    wrefresh(status_win);
    wrefresh(name_win);
    wmove(command_win, 0, 0);
    wclear(command_win);
    if(ed.Bar != NULL){
        wprintw(command_win, "%s", ed.Bar);
    }
    wrefresh(command_win);
    if(ed.mode != 'C'){
        mymove(ed.y, ed.x);
    }
}


void insert(int input){
    char st[N] = {"\0"};
    for(int i = 0; i < ed.x; i++){
        st[i] = ed.Buffer[ed.y][i];
    }
    st[ed.x] = input;
    for(int i = ed.x; i < strlen(ed.Buffer[ed.y]); i++){
        st[i+1] = ed.Buffer[ed.y][i];
    }
    // mvprintw(10, 10, "(%d)\n", input);
    for(int i = 0; i < strlen(st); i++){
        ed.Buffer[ed.y][i] = st[i];
    }
    ed.x++;
}

void act(int input){
    if(navigate_arrow(input))return;
    if(ed.mode == 'V'){
        if(navigate_hjkl(input))return;
        if(input == 'i'){
            ed.mode = 'I';
        }
        if(input == 27){
            ed.mode = 'N';
        }
        return;
    }
    // exit(0);
    if(ed.mode == 'N'){
        if(navigate_hjkl(input))return;
        if(input == 'i'){
            ed.mode = 'I';
        }
        if(input == 'v'){
            ed.mode = 'V';
        }
        if(input == ':' || input == '/'){
            ed.mode = 'C';
            // ed.Bar[0] = input;
            memset(ed.Bar, '\0', sizeof(ed.Bar));
            ed.Bar[0] = input;
        }
        return;
    }
    if(ed.mode == 'C'){
        if(input == 10){
            ed.mode = 'N';
            perform(ed.Bar);
            ed.Bar[0] = '\0';
            return;
        }
        ed.Bar[strlen(ed.Bar)] = input;
        return;
    }
    if(input == 27){
        ed.mode = 'N';
        return;
    }
    insert(input);
}

int main(int argc, char* argv[]){
    if(argc > 1){
        char* file_n = "";
        file_n = argv[1];
        if(strcmp(file_n, "/root/dir1/file.txt"))return 0;
        init(file_n);
    }else{
        printf("No file selected\n");
        return 0;
    }
    //("%s\n", ed.Buffer);
    curses_init();
    int height, width;
    getmaxyx(stdscr, height, width);
    status_win = newwin(1, 8, height-2, 0);
    name_win = newwin(1, width-8, height-2, 8);
    command_win = newwin(1, width-1, height-1, 0);
    // wattron(status_win, COLOR_PAIR(1));
    // wattron(command_win, COLOR_PAIR(2));
    // wattron(name_win, COLOR_PAIR(3));

    while(true){
        printBuff();
        int input = getch();
        act(input);
    }
    endwin();
    return 0;
}