#include "curses.h"
#include "unistd.h"
#include "limits.h"
#include "string.h"
#include "dirent.h"
#include "stdlib.h"

int max, cursor_index, height, width, max_x, max_y;
WINDOW * win = NULL;

void move_wrap_cursor(const int amount) {
  if (amount > 0) {
    cursor_index = cursor_index < (max - (amount - 1)) ? cursor_index + amount : 0;
  }
  else if (amount < 0) {
    cursor_index = cursor_index > (0 - (amount + 1)) ? cursor_index + amount : max;
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
  while (!exit) {
    if (is_term_resized(max_y, max_x)) {
      update_term_dimensions();
    }
    wclear(win);
    box(win, 0, 0);
    max = 0;
    char pwd[PATH_MAX];
    if (!getcwd(pwd, sizeof(pwd)))
      mvprintw(0, 0, "Unknown");
    else {
      DIR *dir;
      if ((dir = opendir(pwd))) {
        max = 0;
        struct dirent *ent;
        while ((ent = readdir(dir)) && max < 10) {
          char * name = ent->d_name;
          if (strcmp(name, ".") && strcmp(name, "..")) {
            if (max == cursor_index)
              wattron(win, A_REVERSE);
            mvwprintw(win, max + 1, 1, ent->d_name);
            max++;
            wattroff(win, A_REVERSE);
          }
        } 
      }
      closedir(dir);
      char * progress = malloc(sizeof(char) * 10);
      progress[0] = cursor_index + 1 + '0';
      progress[1] = '/';
      progress[2] = max + '0';
      mvwprintw(win, 12, 1, progress);
      free(progress);
      char c = wgetch(win);
      const char * ch = keyname(c);
      max--;
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
          cursor_index = max;
          break;
        default :
          if (!strcmp(ch, "^D")) {
            move_cursor(10);
          }
          if (!strcmp(ch, "^U")) {
            move_cursor(-10);
          }
      }
    }
    wrefresh(win);
  }
  endwin();
  return 0;
}
