# CurseFM

An Ncurses file manager written in C. Ranger BTFO.

Very much a work in progress. Just something I do for fun. Don't expect to be completely done.

This also slighlty doubles up as a stab at an ncurses widget library.

# Dependencies

- ncurses
- libmagic-dev - mimetypes for previewing files
- ueberzug - image previews (optional)
- mediainfo - media file previews (optional)

Any optional dependancies are not required at compile time and can be disabled in the configuration file.

# Building

Clone the repo, cd into CurseFM and run make

# TODO

## Must

- Popup prompts for entering text or saying yes/no or just showing warnings
- Create new files
- Copy/Cut/Paste/Delete files
- Highlight multiple files for bulk manipulation
- Handle symlinks
- Custom scripts bound to key presses

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
- Media previews are slow
- Resizing the window does not reset the scrolling amount to match the current index in the file list. It just goes off the screen
