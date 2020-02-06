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

/* Size of the gap that separates the two main areas of the screen
 * Typical values will be either 0(no gap) or 1(small gap) but can be
 * arbitrary
 * Default 1*/
#define WINDOW_GAP 1

/* Show a confirmation alert when deleting a file
 * on(1)/off(0)
 * Default 1*/
#define CONFIRM_ON_DELETE 1


/*PREVIEWS*/

/* Show any previews at all. Turning off disables the preview window and makes the
 * file list full screen
 * on(1)/off(0)
 * Default 1*/
#define SHOW_PREVIEWS 1

/* Show the contents of the currently selected folder in the preview pane
 * on(1)/off(0)
 * Default 1*/
#define SHOW_FOLDER_PREVIEWS 1

/* Show the contents of the currently selected file in the preview pane
 * on(1)/off(0)
 * Default 1*/
#define SHOW_FILE_PREVIEWS 1

/* Display the currently selected image file in the preview pane
 * on(1)/off(0)
 * Default 1*/
#define SHOW_IMAGE_PREVIEWS 1

/* Display the currently selected media file in the preview pane
 * on(1)/off(0)
 * Default 1*/
#define SHOW_MEDIA_PREVIEWS 1

/* Program used to preview media files
 * Needs to output info as text to stdout
 * Default "mediainfo"*/
#define MEDIA_PREVIEW_COMMAND "mediainfo"

/* Script used for displaying image previews
 * Default "/usr/share/curse_fm/ueberzug"*/
#define IMAGE_PREVIEW_SCRIPT "/usr/share/curse_fm/ueberzug"

/* Script used for clearing image previews
 * Default "/usr/share/curse_fm/ueberzug_clear"*/
#define IMAGE_PREVIEW_CLEAR_SCRIPT "/usr/share/curse_fm/ueberzug_clear"


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

/* Delete the current file or the file in the highlighted selection list
 * Default "D"*/
#define DELETE_FILE "D"

/* Highlights the current file and adds it to the current selection list
 * Default " " (spacebar)*/
#define HIGHLIGHT_KEY " "

/* Clears the list of highlighted files
 * DEFAULT "c"*/
#define CLEAR_SELECTION "c"

