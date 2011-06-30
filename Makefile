all: nobox.c
	gcc nobox.c -o nobox -std=gnu99 -O1 -s -lxcb -march=native -fno-inline -fno-stack-protector -fpredictive-commoning -fwhole-program
