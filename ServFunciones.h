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