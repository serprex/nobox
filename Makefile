NBX=${CC} nobox.c -o nobox -std=gnu99 -Os -s -lxcb -march=native -fno-stack-protector -fwhole-program
nobox:nobox.c
	${NBX}
composite:nobox.c
	${NBX} -DCOMPOSITE -lxcb-composite