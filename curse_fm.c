#include "curses.h"
#include "unistd.h"
#include "limits.h"
#include "string.h"
#include "dirent.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "magic.h"
#include "signal.h"
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"

int max, cursor_index, height, width, max_x, max_y;
char current_directory[PATH_MAX];
WINDOW *file_win = NULL;
WINDOW *preview = NULL;
WINDOW *title = NULL;
struct dirent **files;
char go_to[PATH_MAX];
int show_hidden = SHOW_HIDDEN;
int reprint = 0;
int scroll_amount = 0;
int preview_image = 0;
const int title_height = 1;
const int x_border_offset = 2;
const int y_border_offset = 2 + title_height;

void open_file(char *prog, char *file) {
  int pid = fork();
  if (pid == 0) {
    int null_fd = open("/dev/null", O_WRONLY);
    dup2(null_fd,2);
    dup2(null_fd,1);
    char path[PATH_MAX];
    strcpy(path, current_directory);
    strcat(path, file);
    execlp(prog, prog, path, (char*)NULL);
    exit(0);
  }
}

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
      if ((cursor_index + amount) > height - title_height)
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
  wborder(file_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wborder(preview, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  werase(file_win);
  werase(preview);
  werase(title);
  delwin(file_win);
  delwin(preview);
  delwin(title);
  getmaxyx(stdscr,max_y,max_x);
  int factor = SHOW_PREVIEWS ? 2 : 1;
  height = max_y - y_border_offset;
  width = (max_x / factor) - x_border_offset;
  int offset = 0;
  if (WINDOW_GAP < 0)
    offset = WINDOW_GAP;
  file_win = newwin(max_y - title_height, (max_x / factor) + offset, title_height, 0);
  if (SHOW_PREVIEWS)
    preview = newwin(max_y - title_height, max_x / 2 + 2 - WINDOW_GAP, title_height, width + WINDOW_GAP + 1);
  title = newwin(title_height, max_x, 0, 0);
  nodelay(file_win, 1);
  reprint = 1;
}

/* Prints files to the given window from an array of dirent pointers. Starts at a given start
 * value and ends until either the safety value or the height of the screen is reached.
 * The safety value should be the size of the array. Also indeicates directories by colouring
 * and bolding them and indeicates the cursor by inverting the background and foreground colours.*/
void print_files(WINDOW * win, struct dirent **file_list, const int start, const int safety, int draw_cursor, int include_scrolling) {
  for (int i = start; i < start + safety && i < height + 1; i++) {
    include_scrolling = include_scrolling ? scroll_amount : 0;
    const struct dirent *file = file_list[i - start + include_scrolling];
    if (is_directory(file)) {
      wattron(win, COLOR_PAIR(1));
      wattron(win, A_BOLD);
    }
    if (i - 1 == cursor_index && draw_cursor)
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
  if (cursor_index + scroll_amount > (max - 1) || max == 0)
    return;
  struct dirent *dir = files[cursor_index + scroll_amount];
  if (!is_directory(dir)) {
    reprint = 0;
    open_file(OPENER, dir->d_name);
    return;
  }
  char test[PATH_MAX];
  strcpy(test,current_directory);
  strcat(test, dir->d_name);
  DIR * test_dir;
  if (!(test_dir = opendir(test))) {
    reprint = 0;
    return;
  }
  closedir(test_dir);
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

void clear_image() {
  if (preview_image) {
    kill(preview_image, SIGTERM);
    int status;
    waitpid(preview_image,&status, 0);
    int pid = fork();
    if (!pid) {
      execl(IMAGE_PREVIEW_CLEAR_SCRIPT, IMAGE_PREVIEW_CLEAR_SCRIPT, (char*)NULL);
      exit(0);
    }
    waitpid(pid, &status, 0);
  }
}

/* Prints a preview of the currently selected file in the preview window
 * if available*/
void print_preview() {
  clear_image();
  if (!max || (cursor_index + scroll_amount) >= max)
    return;
  struct dirent *ent = files[cursor_index + scroll_amount];
  char current_file[PATH_MAX];
  strcpy(current_file, current_directory);
  strcat(current_file, ent->d_name);
  /*Directory previews*/
  if (is_directory(ent) && SHOW_FOLDER_PREVIEWS) {
    DIR *dir;
    if (!(dir = opendir(current_file)))
      return;
    struct dirent **sub_files;
    int limit = scandir(current_file, &sub_files, selector, compare_files);
    closedir(dir);
    print_files(preview, sub_files, 1, limit, 0, 0);
    free_files(sub_files, limit);
  }
  else {
    /*Get the mime type of the file*/
    const char *mime;
    magic_t magic;
    magic = magic_open(MAGIC_MIME_TYPE); 
    magic_load(magic, NULL);
    mime = magic_file(magic, current_file);
    /*Text file preview*/
    if (strstr(mime, "text") && SHOW_FILE_PREVIEWS) {
      char buff[width + x_border_offset];
      FILE *stream;
      if ((stream = fopen(current_file, "r"))) {
        int i = 1;
        while (fgets(buff, width + x_border_offset, stream) && i < height + y_border_offset) {
          mvwprintw(preview, i, 1, buff);
          i++;
          memset(buff, '0', width + x_border_offset);
        }
      fclose(stream);
      }
    }
    /*Image previews*/
    else if (strstr(mime, "image") && SHOW_IMAGE_PREVIEWS) {
      preview_image = fork();
      if (!preview_image) {
        int null_fd = open("/dev/null", O_WRONLY);
        dup2(null_fd,0);
        dup2(null_fd,2);
        char x[5];
        char y[5];
        char mx[5];
        char my[5];
        sprintf(x ,"%d", width + 2 + WINDOW_GAP);
        sprintf(y ,"%d", title_height + 1);
        sprintf(mx ,"%d", width - WINDOW_GAP + 1);
        sprintf(my ,"%d", height - 2);
        execl(IMAGE_PREVIEW_SCRIPT, IMAGE_PREVIEW_SCRIPT, current_file, x, y, mx, my, (char*)NULL);
        exit(0);
      }
    }
    magic_close(magic);
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
  init_pair(1, DIR_COL, COLOR_BLACK);
  keypad(file_win, 1);
  update_term_dimensions();
  signal(SIGWINCH, update_term_dimensions);
  if (!getcwd(current_directory, sizeof(current_directory)))
    exit = 1;
  strcat(current_directory, "/");
  /*Main loop*/
  while (!exit) {
    //if (is_term_resized(max_y, max_x)) {
      //update_term_dimensions();
    //}
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
      if (new_index > height - 1) {
        cursor_index = height - 1;
        scroll_amount = new_index - cursor_index;
      }
      else
        cursor_index = new_index;
    }
    /*Print the directory contents only when necessary*/
    if (reprint || max != old_max) {
      /*Create arrays for grouping folders and sorting alphabetically*/
      cursor_index = cursor_index + 1 > max ? max - 1 : cursor_index;
      werase(file_win);
      werase(preview);
      werase(title);
      if (SHOW_PREVIEWS)
        print_preview();
      print_files(file_win, files, 1, max, 1, 1);
      if (BORDERS) {
        box(file_win, 0, 0);
        if (SHOW_PREVIEWS)
          box(preview, 0, 0);
      }
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
    if (!strcmp(ch, EXIT_KEY))
      exit = 1;
    else if (!strcmp(ch, CURSOR_UP_KEY))
      move_cursor(-1, WRAP_CURSOR);
    else if (!strcmp(ch, CURSOR_DOWN_KEY))
      move_cursor(1, WRAP_CURSOR);
    else if (!strcmp(ch, FIRST_INDEX_KEY))
      cursor_index = 0;
    else if (!strcmp(ch, LAST_INDEX_KEY))
      cursor_index = max - 1;
    else if (!strcmp(ch, FORWARD_KEY))
      forward_dir();
    else if (!strcmp(ch, BACKWARD_KEY))
      backward_dir();
    else if (!strcmp(ch, TOGGLE_HIDDEN_KEY)) {
      show_hidden = !show_hidden;
      strcpy(go_to, files[cursor_index + scroll_amount]->d_name);
    }
    else if (!strcmp(ch, PAGE_DOWN_KEY))
      move_cursor(height - 1, 0);
    else if (!strcmp(ch, PAGE_UP_KEY))
      move_cursor(-height + 1, 0);
    else
      reprint = 0;
    usleep(5000);
  }
  clear_image();
  endwin();
  return 0;
}
