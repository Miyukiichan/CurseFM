PROG = curse_fm
PREFIX = /usr
BINDIR = $(PREFIX)/bin
default:
	gcc -o $(PROG) curse_fm.c -lncurses -lmagic
clean:
	rm $(PROG)
install:
	install -Dm 755 $(PROG) $(BINDIR)
uninstall:
	rm -v $(BINDIR)/$(PROG)
