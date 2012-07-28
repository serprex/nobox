composite:nobox.c
	${CC} -DCOMPOSITE nobox.c -o nobox -std=gnu99 -Os -s -lxcb -lxcb-composite -march=native -fno-stack-protector -fwhole-program
nobox:nobox.c
	${CC} nobox.c -o nobox -std=gnu99 -Os -s -lxcb -march=native -fno-stack-protector -fwhole-program