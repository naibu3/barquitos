all: servidor cliente

servidor: servidor.c servFunciones.c
	gcc -o $@ $^

cliente: cliente.c
	gcc -o $@ $^
