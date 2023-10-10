all: servidor cliente

servidor: servidor.c
	gcc -o $@ $^

cliente: cliente.c
	gcc -o $@ $^
