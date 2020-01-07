#include "curses.h"
#include "unistd.h"
#include "limits.h"
#include "string.h"
#include "dirent.h"
#include "stdlib.h"
#include "config.h"

typedef struct File {
  char * name;
  struct File * next;
}FileT;

int max, cursor_index, height, width, max_x, max_y;
int title_height = 1;
char current_directory[PATH_MAX];
WINDOW *win = NULL;
WINDOW *preview = NULL;
WINDOW *title = NULL;
FileT *dirs = NULL; //Folders
FileT *files = NULL; //Everything else
int dir_count = 0;
char go_to[PATH_MAX];
int show_hidden = SHOW_HIDDEN;
int reprint = 0;

/* Moves the cursor down (positive) or up (negative). If it goes past the limit, it will be set too the
 * first item if it is too far down, and the last item if too far up.*/
void move_wrap_cursor(const int amount) {
  if (amount > 0) {
    cursor_index = cursor_index < ((max - 1) - (amount - 1)) ? cursor_index + amount : 0;
  }
  else if (amount < 0) {
    cursor_index = cursor_index > (0 - (amount + 1)) ? cursor_index + amount : (max - 1);
  }
}

/*Same as previous function but stays at the limit as opposed to wrapping around*/
void move_cursor(const int amount) {
  if (amount > 0) {
    cursor_index = cursor_index < (max - (amount - 1)) ? cursor_index + amount : (max - 1);
  }
  else if (amount < 0) {
    cursor_index = cursor_index > (0 - (amount + 1)) ? cursor_index + amount : 0;
  }
}

/*Resize and reinitialize the main window*/
void update_term_dimensions() {
  height = max_y - 2 - title_height;
  width = (max_x / 2) - 2;
  wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  if (win) {
    wrefresh(win);
    delwin(win);
  }
  win = newwin(max_y - title_height, (max_x / 2) - 1, title_height, 0);
  preview = newwin(max_y - title_height, max_x / 2 + 1, title_height, width + 1);
  title = newwin(title_height, max_x, 0, 0);
  nodelay(win, 1);
}

/*File list functions*/
FileT * new_file() {
  FileT *file = (FileT*)malloc(sizeof(FileT));
  file->next = NULL;
  return file;
}

FileT *free_files(FileT *head) {
  while (head) {
    FileT *temp = head->next;
    free(head);
    head = temp;
  }
  return new_file();
}

FileT *append_file(FileT *current, struct dirent *ent) {
  current->name = ent->d_name;
  current->next = new_file();
  return current->next;
}

/* Prints linked list of files starting at a particular offset and going down one by one. 
 * Finishes when linked list is finished or when the safety value is reached (ie. the size of the list)
 * The cursor position is indicated by inverting the background and foreground colours.*/
void print_files(FileT *file, const int start, const int safety) {
  for (int i = start; i < start + safety && i < height && file; i++) {
    if (i - 1 == cursor_index)
      wattron(win, A_REVERSE);
    mvwprintw(win, i, 1, file->name);
    wattroff(win, A_REVERSE);
    file = file->next;
  }
}

/* Gets the name of the current index and appends it to the current path.
 * The path is then automatically switched to in the main loop*/
void forward_dir() {
  if (cursor_index >= dir_count || max == 0) {
    reprint = 0;
    return;
  }
  FileT *dir = dirs;
  for (int i = 0; i < cursor_index && dir; i++) {
    dir = dir->next;
  }
  char test[PATH_MAX];
  strcpy(test,current_directory);
  strcat(test, dir->name);
  if (!opendir(test)) {
    reprint = 0;
    return;
  }
  strcat(current_directory, dir->name);
  strcat(current_directory, "/");
  cursor_index = 0;
}

/* Gets the last occurrance of a "/" and sets the subsequent chars in the 
 * string to be empty*/
void backward_dir() {
  if (!strcmp(current_directory, "/")) {
    reprint = 0;
    return;
  }
  char *last = strrchr(current_directory, '/');
  *last = '\0';
  last = strrchr(current_directory, '/');
  strcpy(go_to, last + 1);
  last++;
  while (*last != '\0') {
    *last = '\0';
    last++;
  }
}

int main(int argc, char **argv) {
  /*Initialise ncurses and global variables*/
  max = 0; //Total amount of files and folders
  cursor_index = 0; //Current item selected
  int exit = 0;
  initscr();
  curs_set(0); //Remove the cmd prompt cursor
  noecho();
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  getmaxyx(stdscr,max_y,max_x);
  update_term_dimensions(max_y, max_x);
  if (!getcwd(current_directory, sizeof(current_directory)))
    exit = 1;
  strcat(current_directory, "/");
  /*Main loop*/
  while (!exit) {
    if (is_term_resized(max_y, max_x)) {
      update_term_dimensions();
    }
    int old_max = max;
    dir_count = 0;
    max = 0;
    DIR *dir;
    if (!(dir = opendir(current_directory))) {
      exit = 1;
      continue;
    }
    /*Clear the list and initialise for next read*/
    files = free_files(files);
    dirs = free_files(dirs);
    FileT *current_file_index = files;
    FileT *current_dir_index = dirs;
    /*Read current directory into linked lists*/
    struct dirent *ent;
    while ((ent = readdir(dir))) {
      char * name = ent->d_name;
      if ((strcmp(name, ".") && strcmp(name, "..")) && (name[0] != '.' || show_hidden)) {
        if (ent->d_type == DT_DIR) {
          current_dir_index = append_file(current_dir_index, ent);
          dir_count++;
        }
        else
          current_file_index = append_file(current_file_index, ent);
        max++;
      }
    }
    closedir(dir);
    if (strlen(go_to)) {
      current_dir_index = dirs;
      cursor_index = 0;
      for (int i = 0; i < dir_count && current_dir_index ; i++) {
        if (!strcmp(current_dir_index->name, go_to)) {
          cursor_index = i;
          break;
        }
        current_dir_index = current_dir_index->next;
      }
      memset(go_to,0,strlen(go_to));
    }
    /*Print the directory contents only when necessary*/
    if (reprint || max != old_max) {
      /*Create arrays for grouping folders and sorting alphabetically*/
      cursor_index = cursor_index + 1 > max ? max - 1 : cursor_index;
      int file_count = max - dir_count;
      current_file_index = files;
      current_dir_index = dirs;
      wclear(win);
      wclear(preview);
      wclear(title);
      if (BORDERS) {
        box(win, 0, 0);
        box(preview, 0, 0);
      }
      wattron(win, COLOR_PAIR(1));
      wattron(win, A_BOLD);
      print_files(dirs, 1, dir_count);
      wattroff(win, COLOR_PAIR(1));
      wattroff(win, A_BOLD);
      print_files(files, dir_count + 1, file_count);
      char progress[10];
      sprintf(progress, "[%d/%d]", cursor_index + 1, max);
      int offset = strlen(current_directory) + 2;
      mvwprintw(title, 0, offset, progress);
      mvwprintw(title, 0, 1, current_directory);
      wrefresh(title);
      wrefresh(preview);
    }
    reprint = 1;
    /*Key input*/
    char c = wgetch(win);
    const char * ch = keyname(c);
    switch (c) {
      case 'q':
        exit = 1;
        break;
      case 'k':
        move_wrap_cursor(-1);
        break;
      case 'j':
        move_wrap_cursor(1);
        break;
      case 'g':
        cursor_index = 0;
        break;
      case 'G':
        cursor_index = max - 1;
        break;
      case 'l':
        forward_dir();
        break;
      case 'h':
        backward_dir();
        break;
      case 'z':
        show_hidden = !show_hidden;
        break;
      default :
        if (!strcmp(ch, "^D")) {
          move_cursor(height - 1);
        }
        else if (!strcmp(ch, "^U")) {
          move_cursor(-height + 1);
        }
        else
          reprint = 0;
    }
    usleep(5000);
  }
  endwin();
  return 0;
}
