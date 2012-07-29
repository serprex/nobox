NBX=${CC} nobox.c -o nobox -std=gnu99 -Os -s -lxcb -march=native -fno-stack-protector -fwhole-program
composite:
	${NBX} -DCOMPOSITE -lxcb-composite
nobox:
	${NBX}