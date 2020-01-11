#include "curses.h"
#include "unistd.h"
#include "limits.h"
#include "string.h"
#include "dirent.h"
#include "stdlib.h"
#include "config.h"

int max, cursor_index, height, width, max_x, max_y;
int title_height = 1;
char current_directory[PATH_MAX];
WINDOW *file_win = NULL;
WINDOW *preview = NULL;
WINDOW *title = NULL;
struct dirent **files;
char go_to[PATH_MAX];
int show_hidden = SHOW_HIDDEN;
int reprint = 0;
int scroll_amount = 0;

int is_directory(const struct dirent *ent) {
  return ent->d_type == DT_DIR;
}

/*Prioritises directories to be grouped at the top. Each group is then
 * alphabetized.*/
int compare_files(const struct dirent **a, const struct dirent **b) {
  const struct dirent *ap = *a;
  const struct dirent *bp = *b;
  int alphacmp = alphasort(a, b);
  if (!is_directory(ap) && is_directory(bp)) {
    return 1;
  }
  if (is_directory(ap) == is_directory(bp)) {
    if (alphacmp > 0)
      return 1;
    return -1;
  }
  return -1;
}

/* Selection filter callback for the scandir function. Only shows hidden files when user
 * chooses and completely removes the current and previous directory entries from the list*/
int selector(const struct dirent *ent) {
  return (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) && (ent->d_name[0] != '.' || show_hidden);
}

/* Moves the cursor down (positive) or up (negative). If it goes past the limit, it will be set too the
 * first item if it is too far down, and the last item if too far up (Only of wrap is true). 
 * Otherwise it stays at the limit*/
void move_cursor(const int amount, int wrap) {
  if (amount > 0) {
    if (cursor_index + scroll_amount < ((max - 1) - (amount - 1))) {
      if ((cursor_index + amount) > height - 1)
        scroll_amount += amount;
      else
        cursor_index += amount;
    }
    else {
      if (wrap) {
        cursor_index = 0;
        scroll_amount = 0;
      }
      else {
        cursor_index = (max - 1) > height ? height - 1 : max - 1;
        scroll_amount = (max - 1) > height ? max - height : 0;
      }
    }
  }
  else if (amount < 0) {
    if (cursor_index + scroll_amount > (0 - (amount + 1))) {
      if ((cursor_index + amount) > -1)
        cursor_index += amount;
      else
        scroll_amount += amount;
    }
    else {
      if (wrap) {
        cursor_index = (max - 1) > height ? height - 1 : max - 1;
        scroll_amount = (max - 1) > height ? max - height : 0;
      }
      else {
        cursor_index = 0;
        scroll_amount = 0;
      }
    }
  }
}

/*Resize and reinitialize the main window*/
void update_term_dimensions() {
  height = max_y - 2 - title_height;
  width = (max_x / 2) - 2;
  wborder(file_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  if (file_win) {
    wrefresh(file_win);
    delwin(file_win);
  }
  file_win = newwin(max_y - title_height, (max_x / 2) - 1, title_height, 0);
  preview = newwin(max_y - title_height, max_x / 2 + 1, title_height, width + 1);
  title = newwin(title_height, max_x, 0, 0);
  nodelay(file_win, 1);
}

/* Prints files to the given window from an array of dirent pointers. Starts at a given start
 * value and ends until either the safety value or the height of the screen is reached.
 * The safety value should be the size of the array. Also indeicates directories by colouring
 * and bolding them and indeicates the cursor by inverting the background and foreground colours.*/
void print_files(WINDOW * win, struct dirent **file, const int start, const int safety) {
  for (int i = start; i < start + safety && i < height + 1; i++) {
    const struct dirent *file = files[i - start + scroll_amount];
    if (is_directory(file)) {
      wattron(win, COLOR_PAIR(1));
      wattron(win, A_BOLD);
    }
    if (i - 1 == cursor_index)
      wattron(win, A_REVERSE);
    mvwprintw(win, i, 1, file->d_name);
    wattroff(win, COLOR_PAIR(1));
    wattroff(win, A_BOLD);
    wattroff(win, A_REVERSE);
  }
}

/* Gets the name of the current index and appends it to the current path.
 * The path is then automatically switched to in the main loop*/
void forward_dir() {
  if (cursor_index + scroll_amount > (max - 1))
    return;
  struct dirent *dir = files[cursor_index + scroll_amount];
  if (!is_directory(dir)) {
    reprint = 0;
    return;
  }
  char test[PATH_MAX];
  strcpy(test,current_directory);
  strcat(test, dir->d_name);
  if (!opendir(test)) {
    reprint = 0;
    return;
  }
  strcat(current_directory, dir->d_name);
  strcat(current_directory, "/");
  cursor_index = 0;
  scroll_amount = 0;
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

void free_files(struct dirent ** file_list, int size) {
  for (int i = 0; i < size; i++)
    free(file_list[i]);
  free(file_list);
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
    max = 0;
    DIR *dir;
    if (!(dir = opendir(current_directory))) {
      exit = 1;
      continue;
    }
    free_files(files, old_max);
    max = scandir(current_directory, &files, selector, compare_files);
    closedir(dir);
    if (strlen(go_to)) {
      cursor_index = 0;
      scroll_amount = 0;
      int new_index = 0;
      for (int i = 0; i < max; i++) {
        struct dirent *d = files[i];
        if (!strcmp(d->d_name, go_to)) {
          new_index = i;
          break;
        }
      }
      memset(go_to,0,strlen(go_to));
      move_cursor(new_index, 0);
    }
    /*Print the directory contents only when necessary*/
    if (reprint || max != old_max) {
      /*Create arrays for grouping folders and sorting alphabetically*/
      cursor_index = cursor_index + 1 > max ? max - 1 : cursor_index;
      werase(file_win);
      werase(preview);
      werase(title);
      if (BORDERS) {
        box(file_win, 0, 0);
        box(preview, 0, 0);
      }
      print_files(file_win, files, 1, max);
      char progress[10];
      sprintf(progress, "[%d/%d]", cursor_index + scroll_amount + 1, max);
      int offset = strlen(current_directory) + 2;
      mvwprintw(title, 0, offset, progress);
      mvwprintw(title, 0, 1, current_directory);
      wrefresh(title);
      wrefresh(preview);
    }
    reprint = 1;
    /*Key input*/
    char c = wgetch(file_win);
    const char * ch = keyname(c);
    switch (c) {
      case 'q':
        exit = 1;
        break;
      case 'k':
        move_cursor(-1, 1);
        break;
      case 'j':
        move_cursor(1, 1);
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
        //strcpy(go_to, files[cursor_index + scroll_amount]->d_name);
        break;
      default :
        if (!strcmp(ch, "^D")) {
          move_cursor(height - 1, 0);
        }
        else if (!strcmp(ch, "^U")) {
          move_cursor(-height + 1, 0);
        }
        else
          reprint = 0;
    }
    usleep(5000);
  }
  endwin();
  return 0;
}
