default:
	gcc -o curse_fm curse_fm.c -lncurses -lmagic
debug:
	gcc -g -o curse_fm curse_fm.c -lncurses -lmagic
clean:
	rm curse_fm
