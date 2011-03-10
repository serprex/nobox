all: nobox.c
	gcc nobox.c -o nobox -std=gnu99 -Os -s -lxcb -march=native -fno-inline -fgcse-sm -fgcse-las -fgcse-after-reload -fno-stack-protector -funsafe-loop-optimizations -fpredictive-commoning -fwhole-program
