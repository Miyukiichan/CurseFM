PROG = curse_fm
PREFIX = /usr
BINDIR = $(PREFIX)/bin
SHARE = /usr/share/curse_fm
default:
	gcc -o $(PROG) curse_fm.c -lncursesw -lmagic
clean:
	rm $(PROG)
install:
	install -Dm 755 $(PROG) $(BINDIR)
	install -Dm 755 ueberzug $(SHARE)/ueberzug
	install -Dm 755 ueberzug_clear $(SHARE)/ueberzug_clear
uninstall:
	rm -v $(BINDIR)/$(PROG)
	rm -v $(SHARE)/ueberzug
	rm -v $(SHARE)/ueberzug_clear
