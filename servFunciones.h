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

struct Barco{

	int vida;
	int longitud;
	int posiciones[4][2];	//row, col
};

struct Partida{

    int id_partida;
	char tablero1[10][10];
	struct Barco barcos1[5];
	char tablero2[10][10];
	struct Barco barcos2[5];
    int j1;
    int j2;
	int turno;
	int nDisparos;
};

/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);

/*-------------------------------------
    LOGIN
---------------------------------------*/

int JugadorConectado(struct Jugador jugadores[30], int nJugadores, char nombre[100]);

/*BuscarJugador

	PARAMETROS:
		- a -> Jugador a buscar
	
	RETURN:
		- 1 en caso de éxito ó 0 en caso de error

	Busca el nombre de un jugador en un fichero y devuelve 1 si lo encuentra o 0 si no.
*/
int BuscarJugador( char nombre[100]);

/*RellenaFichero

	PARAMETROS:
		- a -> Jugador a guardar
	
	RETURN:
		- 0 en caso de éxito ó 1 en caso de error

	Guarda un jugador en un fichero y devuelve 0 si lo guarda o 1 si no.
*/
int RellenaFichero(struct Jugador a);

/*CheckPassword

	PARAMETROS:
		- newJ -> Jugador a comprobar
	
	RETURN:
		- 0 en caso de éxito ó 1 en caso de error

	Guarda un jugador en un fichero y devuelve 0 si lo guarda o 1 si no.
*/
int CheckPassword(struct Jugador newJ);

//struct Jugador GetJugador(struct Jugador jugadores[30], int nJugadores, int sd);

int GetPosJugador(struct Jugador jugadores[30], int nJugadores, int sd);

//int ExisteUsuario(i,&readfds,&numClientes,arrayClientes);

int getPartidaJugador(struct Partida[15], int nPartidas, int sd);

/*-------------------------------------
    TABLERO
---------------------------------------*/

/*imprimirTablero
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres

	Imprime el tablero pasado como parámetro.
*/
void imprimirTablero(char tablero[10][10]);

void copiaTablero(char tableroDest[10][10], char tableroOrig[10][10]);

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
void rellenaTablero(char tablero[10][10], struct Barco barcos[5]);

/*colocaBarcoHoriz
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres
		- longitud -> Longitud del barco dada como entero
	
	RETURN:
		- Devuelve 0 en caso de éxito ó 1 en caso de error.

	Coloca un barco de longitud dada horizontalmente en el tablero,
	tiene en cuenta que no haya barcos alrededor.
*/
int colocaBarcoHoriz(char tablero[10][10], int longitud, struct Barco * barco);

/*colocaBarcoVert
	PARAMETROS:
		- tablero -> Matriz de 10x10 caracteres
		- longitud -> Longitud del barco dada como entero
	
	RETURN:
		- Devuelve 0 en caso de éxito ó 1 en caso de error.

	Coloca un barco de longitud dada verticalmente en el tablero,
	tiene en cuenta que no haya barcos alrededor.
*/
int colocaBarcoVert(char tablero[10][10], int longitud, struct Barco * barco);

int letterToInt(char c);

int checkHundido(struct Barco barcos[5], int row, int col);

int checkFin(struct Barco barcos[5]);