# CurseFM

An Ncurses file manager written in C. Ranger BTFO.

# Dependencies

- ncurses
- libmagic-dev (mimetypes)
- ueberzug (for image previews)

# Building

Clone the repo, cd into CurseFM and run make

# TODO

- Scrolling
  - Retain cursor + scroll position for next/previous directories recursively
- Navigate folders
  - Show error message if folder inaccessible
- Copy/Cut/Paste/Delete
- Colours
  - Customizable
  - Colours for different filetypes
- PDF previews
- Handle symlinks
- Watch for directory changes as opposed to reading constantly
  - inotify
- Handle terminal resizing better
  - Current sigwinch attempts don't work

# Bugs

- Showing hidden files during runtime retains the cursor index as opposed to locating the actual item the cursor was on
