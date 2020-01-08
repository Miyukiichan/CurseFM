# CurseFM

An Ncurses file manager written in C. Ranger BTFO.

# TODO

- Sort files and folders alphabetically
- Scrolling
  - Retain cursor + scroll position for next/previous directories recursively
- Navigate folders
  - Show error message if folder inaccessible
- Copy/Cut/Paste
- Custom shortcuts
- Colours
  - Customizable
  - Colours for different filetypes
- Open files
- File/image previews
  - Ueberzug

# Bugs

- Resizing the terminal window causes segfault
- Scrolling in folder with large amount of subfolders and files results in files becomming invisible
  - Due to automatically skipping over files due to the scroll amount in the print function
  - Need to join the directory and file lists for the print
