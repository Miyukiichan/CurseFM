#include "curses.h"
#include "unistd.h"
#include "limits.h"
#include "string.h"
#include "dirent.h"
#include "stdlib.h"

typedef struct File {
  char * name;
  int is_directory;
  struct File * next;
}FileT;

int max, cursor_index, height, width, max_x, max_y;
WINDOW * win = NULL;

void move_wrap_cursor(const int amount) {
  if (amount > 0) {
    cursor_index = cursor_index < ((max - 1) - (amount - 1)) ? cursor_index + amount : 0;
  }
  else if (amount < 0) {
    cursor_index = cursor_index > (0 - (amount + 1)) ? cursor_index + amount : (max - 1);
  }
}

void move_cursor(const int amount) {
  if (amount > 0) {
    cursor_index = cursor_index < (max - (amount - 1)) ? cursor_index + amount : max;
  }
  else if (amount < 0) {
    cursor_index = cursor_index > (0 - (amount + 1)) ? cursor_index + amount : 0;
  }
}

void update_term_dimensions() {
  height = max_y - 2;
  width = max_x - 2;
  wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  if (win) {
    wrefresh(win);
    delwin(win);
  }
  win = newwin(max_y, max_x, 0, 0);
  nodelay(win, 1);
}

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

void print_files(FileT *file, const int start, const int safety) {
  for (int i = 0; i < safety && file; i++) {
    if (i + start - 1 == cursor_index)
      wattron(win, A_REVERSE);
    mvwprintw(win, start + i, 1, file->name);
    wattroff(win, A_REVERSE);
    file = file->next;
  }
}

int main(int argc, char **argv) {
  /*Initialise ncurses and global variables*/
  max = 0;
  cursor_index = 0;
  int exit = 0;
  initscr();
  curs_set(0);
  noecho();
  start_color();
  getmaxyx(stdscr,max_y,max_x);
  update_term_dimensions(max_y, max_x);
  char pwd[PATH_MAX];
  FileT *files = NULL;
  FileT *dirs = NULL;
  if (!getcwd(pwd, sizeof(pwd)))
    exit = 1;
  int reprint = 0;
  /*Main loop*/
  while (!exit) {
    if (is_term_resized(max_y, max_x)) {
      update_term_dimensions();
    }
    int old_max = max;
    int dir_count = 0;
    max = 0;
    DIR *dir;
    if (!(dir = opendir(pwd))) {
      exit = 1;
      continue;
    }
    /*Clear the list and initialise for next read*/
    files = free_files(files);
    dirs = free_files(dirs);
    FileT *current_file_index = files;
    FileT *current_dir_index = dirs;
    /*Read current directory into linked list*/
    struct dirent *ent;
    while ((ent = readdir(dir))) {
      char * name = ent->d_name;
      if (strcmp(name, ".") && strcmp(name, "..")) {
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
    /*Print the directory contents only when ncessary*/
    if (reprint || max != old_max) {
      /*Create arrays for grouping folders and sorting alphabetically*/
      if (cursor_index + 1 > max)
        cursor_index = max - 1;
      int file_count = max - dir_count;
      current_file_index = files;
      current_dir_index = dirs;
      wclear(win);
      box(win, 0, 0);
      print_files(dirs, 1, dir_count);
      print_files(files, dir_count + 1, file_count);
      char progress[10];
      sprintf(progress, "%d/%d", cursor_index + 1, max);
      mvwprintw(win, height, 1, progress);
    }
    reprint = 1;
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
      default :
        if (!strcmp(ch, "^D")) {
          move_cursor(10);
        }
        if (!strcmp(ch, "^U")) {
          move_cursor(-10);
        }
        else
          reprint = 0;
    }
    wrefresh(win);
    usleep(5000);
  }
  endwin();
  return 0;
}
