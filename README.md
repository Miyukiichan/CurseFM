# CurseFM

An Ncurses file manager written in C. Ranger BTFO.

# Dependencies

- ncurses
- libmagic-dev (mimetypes)
- ueberzug (for image previews)

# Building

Clone the repo, cd into CurseFM and run make

# TODO

## Must

- Handle terminal resizing better
  - Current sigwinch attempts don't work
- Copy/Cut/Paste/Delete
- Handle symlinks

## Should

- Statusbar show/hide
- PDF previews
- Colours
  - Customizable
  - Colours for different filetypes
- Navigate folders
  - Show error message if folder inaccessible
- Watch for directory changes as opposed to reading constantly
  - inotify

## Could

- Statusbar modules
  - Some built in already but should be able to show/hide them
  - Custom modules that are read from the standard output of other programs or scripts such as date/time
  - Modules have their own customizable colour settings
- Scrolling
  - Retain cursor + scroll position for next/previous directories recursively

# Bugs

- Printing certain files in the preview causes segfault for some unknown reason
