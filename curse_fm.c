#include "curses.h"
#include "unistd.h"
#include "limits.h"
#include "string.h"
#include "dirent.h"
#include "stdlib.h"

typedef struct File {
  char * name;
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
}

int main(int argc, char **argv) {
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
  if (!getcwd(pwd, sizeof(pwd)))
    exit = 1;
  int reprint = 0;
  while (!exit) {
    if (is_term_resized(max_y, max_x)) {
      update_term_dimensions();
    }
    int old_max = max;
    max = 0;
    DIR *dir;
    if ((dir = opendir(pwd))) {
      struct dirent *ent;
      FileT *current_file_index = files;
      while (current_file_index) {
        FileT *temp = current_file_index->next;
        free(current_file_index);
        current_file_index = temp;
      }
      files = new_file();
      current_file_index = files;
      while ((ent = readdir(dir))) {
        char * name = ent->d_name;
        if (strcmp(name, ".") && strcmp(name, "..")) {
          current_file_index->name = name;
          current_file_index->next = new_file();
          current_file_index = current_file_index->next;
          max++;
        }
      }
      if (reprint || max != old_max) {
        wclear(win);
        box(win, 0, 0);
        current_file_index = files;
        for (int i = 0; i < max && current_file_index; i++) {
          if (i == cursor_index)
            wattron(win, A_REVERSE);
          mvwprintw(win, i + 1, 1, current_file_index->name);
          wattroff(win, A_REVERSE);
          current_file_index = current_file_index->next;
        }
      }
      closedir(dir);
      char progress[10];
      sprintf(progress, "%d/%d", cursor_index + 1, max);
      mvwprintw(win, height, 1, progress);
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
    }
    wrefresh(win);
  }
  endwin();
  return 0;
}
