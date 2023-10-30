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

/*struct Barco
	PARAMETROS
		- vida -> posiciones restantes del barco.
		- longitud -> longitud del barco.
		- posiciones -> vector de pares de casillas con las coordenadas del barco.

	Representación de un barco.
*/
struct Barco{

	int vida;
	int longitud;
	int posiciones[4][2];	//row, col
};

/*struct Barco
	PARAMETROS
		- id_partida -> posiciones restantes del barco.
		- tablero1 -> tablero del jugador 1.
		- barcos1 -> vector con los barcos del jugador1.
		- tablero2 -> tablero del jugador 2.
		- barcos2 -> vector con los barcos del jugador2.
    	- j1 -> id del jugador 1.
		- j2 -> id del jugador 2.
		- turno -> id del jugador que tiene el turno.
		- nDisparos -> numero de disparos.

	Representación de una partida.
*/
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

/*-------------------------------------
	SERVIDOR
---------------------------------------*/

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);

/*-------------------------------------
    LOGIN
---------------------------------------*/

/*JugadorConectado

	PARAMETROS:
		- jugadores -> vector de jugadores.
		- nJugadores -> cantidad de jugadores en el vector.
		- nombre -> nombre del jugador a buscar.

	RETURN:
		- posicion del jugador en el vector.

	Comprueba si un jugador dado por su nombre está en un vector de jugadores.
*/
int JugadorConectado(struct Jugador jugadores[30], int nJugadores, char nombre[100]);

/*BuscarJugador

	PARAMETROS:
		- a -> Jugador a buscar
	
	RETURN:
		- 1 en caso de éxito ó 0 en caso de error

	Busca el nombre de un jugador en un fichero y devuelve 1 si lo encuentra o 0 si no.
*/
int BuscarJugador(char nombre[100]);

/*RellenaFichero

	PARAMETROS:
		- a -> Jugador a guardar
	
	RETURN:
		- 1 en caso de éxito ó 0 en caso de error

	Guarda un jugador en un fichero y devuelve 1 si lo guarda o 0 si no.
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

/*getPosJugador

	PARAMETROS:
		- jugadores -> vector de jugadores
		- nJugadores -> numero de jugadores en el vector
		- sd -> identificador a buscar
	
	RETURN:
		- posicion del jugador en el vector en caso de éxito ó -1 en caso de error

	Busca la posición de un jugador en un vector en base a su sd.
*/
int GetPosJugador(struct Jugador jugadores[30], int nJugadores, int sd);

/*getPartidaJugador

	PARAMETROS:
		- partidas -> vector de partidas
		- nPartidas -> numero de partidas en el vector
		- sd -> identificador a buscar
	
	RETURN:
		- posicion de la partida en el vector en caso de éxito ó -1 en caso de error

	Busca la posición de una partida en un vector en base al sd de uno de los jugadores.
*/
int getPartidaJugador(struct Partida partidas[15], int nPartidas, int sd);

/*-------------------------------------
    JUEGO
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

/*letterToInt

	Convierte una letra c a su posición en el abecedario.
*/
int letterToInt(char c);

/*checkHundido

	Comprueba si un barco se ha tocado:

		- 0 -> TOCADO Y HUNDIDO
		- >0 -> TOCADO
		- -1 -> AGUA
*/
int checkHundido(struct Barco barcos[5], int row, int col);

/*checkFin

	Comprueba si queda algun barco a flote.
*/
int checkFin(struct Barco barcos[5]);