#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<unistd.h>
#include<ncurses.h>
#include "functions.h"

#define distance_from_left 4

void swap(int *x, int *y){
    int tmp = *x;
    *x = *y;
    *y = tmp;
}


struct Editor{
    bool is_saved;
    int x, y;
    int lcnt, starting_line;
    char *file_name;
    char mode;
    char buffer[N][N];
    char command_bar[N];
}editor, ed_backup, tmp;

WINDOW *status_win,*command_win;
bool flag1[N][N], flag2[N][N];
int start_x, start_y, height, width;
char selected_string[N];

void navigation(int input){

    editor.x -= (input == 'h' || input == KEY_LEFT);
    editor.x += (input == 'l' || input == KEY_RIGHT);

    if(input == 'k' || input == KEY_UP){
        if(editor.y == 0 && editor.starting_line != 0 )editor.starting_line--;
        editor.y--;
        if(editor.y<0)editor.y = 0;
    }
    if(input == 'j' || input == KEY_DOWN){
        editor.starting_line += (editor.y == 9);
        
        editor.y++;
        if(editor.y > 9)editor.y = 9;
    }
    if(editor.x > strlen(editor.buffer[editor.y + editor.starting_line])+distance_from_left){
        editor.x = strlen(editor.buffer[editor.y + editor.starting_line])+distance_from_left;
    }
    if(editor.x < distance_from_left){
        editor.x = distance_from_left;
    }
}

void clear_buff(){
    for(int i = 0; i < editor.lcnt; i++){
        memset(editor.buffer[i], '\0', sizeof editor.buffer[i]);
    }
    editor.lcnt = 0;
}

void string_to_editor(char* st){
    clear_buff();
    for(int i = 0; i < strlen(st); i++){
        int cnt = 0;
        for(;i < strlen(st);i++){
            if(st[i] == '\n')break;
            editor.buffer[editor.lcnt][cnt++] = st[i];
        }
        editor.lcnt++;
    }
}

void init(char *file_n){
   editor.file_name = file_n;
   editor.mode = 'n';
   editor.is_saved = 1;
   editor.x = distance_from_left;
   memset(editor.command_bar, '\0', sizeof editor.command_bar);
   wclear(command_win);
   wclear(status_win);
   clear();
   editor.starting_line = editor.y = editor.lcnt = 0;
   if(file_n == NULL)return;
   if(file_n[0] == '/')file_n++;
   FILE *file;
   file = fopen(file_n, "r");

   char st[N] = {"\0"};
   file_to_given_string(st, file);
   string_to_editor(st);
   fclose (file);
}

bool in_range(int y, int x){
    if(editor.mode != 'v')return 0;
    int ly = editor.y;
    int ry = start_y - editor.starting_line;
    int lx = editor.x;
    int rx = start_x;
    if(ly > ry || (ly == ry && lx > rx)){
        swap(&ly, &ry);
        swap(&lx, &rx);
    }
    int lft = N*ly + lx;
    int rit = N*ry + rx;
    int cur = N*y + x;
    return (lft <= cur && cur <= rit);
}

void printBuff(){
   clear();
   if(editor.mode == 'v')memset(selected_string, '\0', sizeof(selected_string));
   for(int i = editor.starting_line; i < editor.starting_line + height - 6; i++){
      // if(i >= editor.lcnt)break;
      move(i-editor.starting_line, 0);
      printw("%3d ", i);
      for(int j = 0; j < strlen(editor.buffer[i]); j++){
         int y, x;
         getyx(stdscr, y, x);
         if(in_range(y, x)){
            selected_string[strlen(selected_string)] = editor.buffer[i][j];
            if(j == strlen(editor.buffer[i]) - 1)selected_string[strlen(selected_string)] = '\n';
            attron(COLOR_PAIR(1));
            addch(editor.buffer[i][j]);
            attroff(COLOR_PAIR(1));
            continue;
         }
         if(flag1[i][j] && editor.mode == 'f'){
            attron(COLOR_PAIR(2));
            addch(editor.buffer[i][j]);
            attroff(COLOR_PAIR(2));
            continue;
         }    
         printw("%c", editor.buffer[i][j]);
         
      }
      printw("\n");
   }
   wclear(status_win);
   wclear(command_win);
   refresh();
   wattron(status_win, COLOR_PAIR(3));
   switch (editor.mode){
      case 'i':
         mvwprintw(status_win, 0, 0, "INSERT");
         break;
      
      case 'v':
         mvwprintw(status_win, 0, 0, "VISUAL");
         break;
      
      default:
         mvwprintw(status_win, 0, 0, "NORMAL");
         break;
   }
   wattroff(status_win, COLOR_PAIR(3));   
   if(editor.file_name == NULL){
        wprintw(status_win, " (unnamed)");
   }else{
       wprintw(status_win, " %s",  editor.file_name);
   }
   if(!editor.is_saved){
      wprintw(status_win, " +");
   }
   wrefresh(status_win);

   wmove(command_win, 0, 0);
   if(editor.command_bar != NULL)wprintw(command_win, "%s", editor.command_bar);
   wrefresh(command_win);

   if(editor.mode != 'm')move(editor.y, editor.x);
}

void insert_input(int input){
   editor.is_saved = false;
   ed_backup = editor;
   char *string = malloc(N*sizeof(char));
   memset(string, '\0', sizeof(string));
   int posy = editor.y + editor.starting_line;
   for(int i = 0; i < editor.x-distance_from_left; i++)string[i] = editor.buffer[posy][i];
   string[editor.x-distance_from_left] = input;
   for(int i = editor.x-distance_from_left; i < strlen(editor.buffer[posy]); i++)string[i+1] = editor.buffer[posy][i];

   strcpy(editor.buffer[posy], string);

   editor.x++;
   if(editor.lcnt < posy+1){
      editor.lcnt = posy+1;
   }
}

void remove_str2(){
   editor.is_saved = 0;
   ed_backup = editor;
   char *string = malloc(N*sizeof(char));
   memset(string, '\0', sizeof(string));
   for(int i = editor.starting_line; i < editor.starting_line + height - 6; i++){
      bool myflag = false;
      for(int j = 0; j < strlen(editor.buffer[i]); j++){
         int y, x;
         getyx(stdscr, y, x);
         if(!in_range(i-editor.starting_line, j+distance_from_left)){
            myflag |= (j == strlen(editor.buffer[i])-1);
            string[strlen(string)] = editor.buffer[i][j];
         }
      }
      if(myflag)string[strlen(string)] = '\n';
   }
   string_to_editor(string);
   editor.x = distance_from_left;
   editor.y = 0;
}


void invalid_command(){
   wclear(command_win);
   strcpy(editor.command_bar, "invalid command");
}

void editor_to_string(char* string){
   int cnt = 0;
   for(int i = 0; i < editor.lcnt; i++){
      for(int j = 0; j < strlen(editor.buffer[i]); j++)
         string[cnt++] = editor.buffer[i][j];
      string[cnt++] = '\n';   
   }
   string[cnt] = '\0';
}



void save(){
   if(editor.file_name == NULL){
      invalid_command();
      return;   
   }
   editor.is_saved = 1;
   char *string = malloc(N*sizeof(char));
   memset(string, '\0', sizeof(string));
   editor_to_string(string);
   write_to_file(string, editor.file_name);
   
   memset(editor.command_bar, '\0', sizeof editor.command_bar); 
   wclear(command_win);
}

void save_as(char *mypath){
   char *string = malloc(N*sizeof(char));
   memset(string, '\0', sizeof(string));
   int length = strlen(mypath);
   if(!create_file(mypath)){
      invalid_command();
      return;
   }
   for(int i = 0; i < length; i++)mypath[i] = editor.command_bar[i+9];
   mypath[length] = '\0';   
   editor_to_string(string);
   write_to_file(string, mypath);
   editor.is_saved = 1;
   editor.file_name = mypath; 
   memset(editor.command_bar, '\0', sizeof editor.command_bar);
   wclear(command_win);
}


void open(char *pt){
   if(pt[0] == '/')pt++;
   if(exists(pt)){
      invalid_command();
      return;
   }
   if(editor.file_name != NULL)save();
   init(pt);
}


void undo_ed(){
   tmp = editor;
   editor = ed_backup;
   ed_backup = tmp;
   editor.mode = 'n';
   editor.is_saved = 0;
   memset(editor.command_bar, '\0', sizeof editor.command_bar);
   wclear(command_win);
}

void clear_found(){
   for(int i = 0; i < editor.lcnt; i++){
      memset(flag1[i], 0, sizeof(flag1[i]));
      memset(flag2[i], 0, sizeof(flag2[i]));
   }
}

void find_expression(char* ptrn){
   ptrn++;
   editor.mode = 'f';
   for(int i = editor.starting_line; i < editor.starting_line + height - 6; i++){
      char *string = editor.buffer[i];
      for(int j = 0; j < strlen(editor.buffer[i]); j++){
         if(!is_prefix(string+j, ptrn))continue;
         flag2[i][j] = 1;
         for(int k = j; k < j+strlen(ptrn); k++){
            flag1[i][k] = 1;
         }
      }
   }
   memset(editor.command_bar, '\0', sizeof editor.command_bar);
   wclear(command_win);
}


void find_next(){
   int y = editor.y;
   int x = editor.x;
   y += editor.starting_line;
   x -= distance_from_left-1;
   for(int i = y; i < editor.starting_line + height-6; i++){
      int start = 0;
      if(i == y)
         start = x+1;
      for(int j = start; j < strlen(editor.buffer[i]); j++){
         if(flag2[i][j]){
            editor.y = i-editor.starting_line;
            editor.x = j+distance_from_left;
            return;
         }
      }
   }  
   for(int i = editor.starting_line; i < editor.starting_line + height-6; i++){
      for(int j = 0; j < strlen(editor.buffer[i]); j++){
         if(flag2[i][j]){
            editor.y = i-editor.starting_line;
            editor.x = j+distance_from_left;
            return;
         }
      }
   }  
}


void auto_indent_editor(){
   editor.is_saved = 0;
   ed_backup = editor;
   char st1[N];
   editor_to_string(st1);
   auto_indent(st1);
   string_to_editor(st1);
   memset(editor.command_bar, '\0', sizeof editor.command_bar);
   wclear(command_win);
}


void act_command(char *command){
   if(command[0] == ':'){
      command++;
      if(strcmp(command, "save") == 0){
         save();
         goto hell;
      }
      char *check = malloc(N*sizeof(char));
      strcpy(check, command);
      check[6] = '\0';
      if(strlen(command) > 9 && (strcmp(check, "saveas") == 0)){
         char *pt = malloc(N*sizeof(char));
         strcpy(pt, editor.command_bar);
         pt += 9;
         save_as(pt);
         wclear(command_win);
         goto hell;
      }
      check = malloc(N*sizeof(char));
      strcpy(check, command);
      check[4] = '\0';
      if(strlen(command) > 6 && (strcmp(check, "open") == 0)){
         char *pt = malloc(N*sizeof(char));
         strcpy(pt, editor.command_bar);
         pt += 6;
         open(pt);
         goto hell;
      }
      if(strcmp(command, "undo") == 0){
         undo_ed();
         goto hell;
      }
      if(strcmp(command, "auto-indent") == 0){
         auto_indent_editor();
         goto hell;
      }
   }
   if(command[0] == '/'){
      find_expression(command);
      goto hell;
   }
   strcpy(editor.command_bar, "invalid command");
   wclear(command_win);
   return;
   hell:
   memset(editor.command_bar, '\0', sizeof editor.command_bar);
   wclear(command_win);
}

void act_V(int input){
   switch (input){
      case 27:
         editor.mode = 'n';
         break;
      
      case (int)'i':
         editor.mode = 'i';
         break;

      case (int)'d':
         clipboard = selected_string;
         remove_str2(editor.file_name);
         editor.mode = 'n';
         break;

      case (int)'y':
         editor.mode = 'n';
         clipboard = selected_string;
         break;
      default:
         navigation(input);
         break;
   }
}

void act_N(int input){
   switch (input){
      case (int)'i':
         editor.mode = 'i';
         break;

      case (int)'v':
         editor.mode = 'v';
         start_x = editor.x;
         start_y = editor.y+editor.starting_line; 
         break;

      case (int)'=':
         auto_indent_editor();
         break;

      case (int)':':
      case (int)'/':
         editor.mode = 'm';
         memset(editor.command_bar, '\0', sizeof editor.command_bar);
         wclear(command_win);
         editor.command_bar[0] = input;
         break;

      case (int)'p':
         editor.is_saved = 0;
         ed_backup = editor;
         for(int i = 0; i < strlen(clipboard); i++){
            insert_input(clipboard[i]);
         }
         break;
      
      case (int)'u':
         undo_ed();
         break;

      default:
         navigation(input);
         break;
   }
}

void act_F(int input){
   switch (input){
      case (int)'n':
         find_next();
         break;
      
      default:
         clear_found();
         editor.mode = 'n';
   }
   return;
}

void act(int input){
   switch (editor.mode){
      case 'n':
         act_N(input);
         break;
      
      case 'v':
         act_V(input);
         break;
      
      case 'f':
         act_F(input);
         break;
      
      case 'm':
         if(input == (int)'\n'){
            editor.mode = 'n';
            act_command(editor.command_bar);
            break;
         }
         editor.command_bar[strlen(editor.command_bar)] = input;
         break;

      default:
         if(input == 27){
            editor.mode = 'n';
            break;
         }
         insert_input(input);
         break;
   }
}




int main(){
   init(NULL);
   initscr();
   noecho();
   cbreak();
   keypad(stdscr, true);
   start_color();
   init_pair(1, COLOR_BLACK, COLOR_CYAN);
   init_pair(2, COLOR_BLACK, COLOR_YELLOW);
   init_pair(3, COLOR_BLACK, COLOR_BLUE);
   getmaxyx(stdscr, height, width);
   status_win = newwin(1, width-1, height-2, 0);
   command_win = newwin(1, width-1, height-1, 0);
   while(true){
      printBuff();
      int input = getch();
      act(input);
   }
   refresh();
   endwin();
   return 0;
}
