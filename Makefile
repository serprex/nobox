all: nobox.c
	gcc nobox.c -o nobox -std=gnu99 -Os -s -lxcb -march=native -fno-stack-protector -fwhole-program
