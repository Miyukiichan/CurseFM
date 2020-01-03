default:
	gcc -o curse_fm curse_fm.c -lncurses
debug:
	gcc -g -o curse_fm curse_fm.c -lncurses
clean:
	rm curse_fm
