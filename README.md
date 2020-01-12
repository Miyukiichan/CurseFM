# CurseFM

An Ncurses file manager written in C. Ranger BTFO.

# TODO

- Scrolling
  - Retain cursor + scroll position for next/previous directories recursively
- Navigate folders
  - Show error message if folder inaccessible
- Copy/Cut/Paste
- Colours
  - Customizable
  - Colours for different filetypes
- File/image previews
  - Ueberzug
- Handle symlinks

# Bugs

- Resizing the terminal window causes segfault
- Showing hidden files during runtime retains the cursor index as opposed to locating the actual item the cursor was on
