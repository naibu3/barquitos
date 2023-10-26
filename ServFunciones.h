#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define MSG_SIZE 250
#define MAX_CLIENTS 30

#include "jugador.h"

struct Partida{

    int id_partida;
    struct Jugador j1;
    struct Jugador j2;
};

/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);
int BuscarJugador( struct jugador a);
int RellenaFichero(struct jugador a);

//int ExisteUsuario(i,&readfds,&numClientes,arrayClientes);

/*-------------------------------------
    TABLERO
---------------------------------------*/

/*imprimirTablero
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres

	Imprime el tablero pasado como parámetro.
*/
void imprimirTablero(char tablero[10][10]);

/*rellenaTablero
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres

	Rellena el tablero pasado como parámetro con:
		- 1 barco de 4 casillas
		- 2 barcos de 3 casillas
		- 2 barcos de 2 casillas
		- El resto de casillas con agua (A)

	Todos aleatoriamente tanto en posicion como orientación (horiz/vert).
*/
void rellenaTablero(char tablero[10][10]);

/*colocaBarcoHoriz
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres
		- longitud -> Longitud del barco dada como entero
	
	RETURN:
		- Devuelve 0 en caso de éxito ó 1 en caso de error.

	Coloca un barco de longitud dada horizontalmente en el tablero,
	tiene en cuenta que no haya barcos alrededor.
*/
int colocaBarcoHoriz(char tablero[10][10], int longitud);

/*colocaBarcoVert
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres
		- longitud -> Longitud del barco dada como entero
	
	RETURN:
		- Devuelve 0 en caso de éxito ó 1 en caso de error.

	Coloca un barco de longitud dada verticalmente en el tablero,
	tiene en cuenta que no haya barcos alrededor.
*/
int colocaBarcoVert(char tablero[10][10], int longitud);