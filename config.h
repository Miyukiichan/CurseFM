/*GENERAL*/

/* Toggle borders
 * on(1)/off(0)
 * Default 0*/
#define BORDERS 1

/* Show hidden files by default. This also can be toggled in the UI.
 * show(1)/hide(0).
 * Default 0*/
#define SHOW_HIDDEN 0

/* Program used to open files
 * Default "xdg-open"*/
#define OPENER "xdg-open"

/* Directory colour
 * Possible colours:
 *   - COLOR_RED
 *   - COLOR_GREEN
 *   - COLOR_YELLOW
 *   - COLOR_BLUE
 *   - COLOR_MAGENTA
 *   - COLOR_CYAN
 *   - COLOR_WHITE
 * Default COLOR_CYAN*/
#define DIR_COL COLOR_CYAN

/* Wrap cursor to the other end of the list automatically
 * when the limit is reached by default
 * on(1)/off(0)
 * Default 1*/
#define WRAP_CURSOR 1

/*KEYBINDS*/

/* Exits the application
 * Default "q"*/
#define EXIT_KEY "q"

/* Go back to the previous directory in the hierarchy
 * Default "h"*/
#define BACKWARD_KEY "h"

/* Go forward a directory (into a sub directory) or open a file with
 * the default opener
 * Default "l"*/
#define FORWARD_KEY "l"

/* Move the cursor up in the file list
 * Default "k"*/
#define CURSOR_UP_KEY "k"

/* Move the cursor down in the file list
 * Default "j"*/
#define CURSOR_DOWN_KEY "j"

/* Move the cursor to the top of the file list
 * Default "g"*/
#define FIRST_INDEX_KEY "g"

/* Move the cursor to the bottom of the file list
 * Default "G"*/
#define LAST_INDEX_KEY "G"

/* Turn hidden files on/off in the file list.
 * Does not alter the setting in this file.
 * Default "z"*/
#define TOGGLE_HIDDEN_KEY "z"

/* Move the cursor up a page in the file list.
 * If there are fewer items remaining compared to the
 * page height the cursor moves to the top.
 * Default "^U" (Ctrl + u)*/
#define PAGE_UP_KEY "^U"

/* Move the cursor down a page in the file list.
 * If there are fewer items remaining compared to the
 * page height the cursor moves to the bottom.
 * Default "^D" (Ctrl + d)*/
#define PAGE_DOWN_KEY "^D"

