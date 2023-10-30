#include "servFunciones.h"

/*-------------------------------------
    CONEXION
---------------------------------------*/

void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]){
  
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente <%d>",socket);
    
    for(j=0; j<(*numClientes); j++)
        if(arrayClientes[j] != socket)
            send(arrayClientes[j],buffer,sizeof(buffer),0);


}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor

}

/*-------------------------------------
    LOGIN
---------------------------------------*/

int JugadorConectado(struct Jugador jugadores[30], int nJugadores,char nombre[100]){

	for(int i=0; i<nJugadores; i++){

		if(strcmp(jugadores[i].nombre, nombre)==0){
			
			return i;
		}
	}

	return -1;
}

int RellenaFichero(struct Jugador newJ){

    FILE *f = fopen("jugadores.txt","a+");

	//ERROR al abrir el fichero
    if(f==NULL){
        perror("Error al abrir el fichero de jugadores\n");
        exit(1);
    }

    struct Jugador j;

    while(fscanf(f,"%s , %s", j.nombre, j.password)==2){

		//printf("USER: %s\n", j.nombre);	//Debug

        if(strcmp(newJ.nombre, j.nombre)==0){

            //perror("El usuario ya ha sido registrado");
            fclose(f);
            return 1;
        }
    }

	fprintf(f, "%s , %s\n", newJ.nombre, newJ.password);

    fclose(f);
    return 0;
}


int BuscarJugador(char nombre[100]){

    FILE *f = fopen("jugadores.txt","a+");

	//ERROR al abrir el fichero
    if(f==NULL){
        perror("Error al abrir el fichero de jugadores\n");
        exit(1);
    }

    struct Jugador j;

    while(fscanf(f, "%s , %s", j.nombre, j.password) == 2){

		//printf("USER: %s\n", j.nombre);	//debug

        if(strcmp(nombre, j.nombre) == 0){

            fclose(f);
            return 1;
        }
        
    }

    fclose(f);
    return 0;

}

int CheckPassword(struct Jugador newJ){

    FILE *f = fopen("jugadores.txt","a+");

	//ERROR al abrir el fichero
    if(f==NULL){
        perror("Error al abrir el fichero de jugadores\n");
        exit(1);
    }

    struct Jugador j;

    while(fscanf(f, "%s , %s", j.nombre, j.password) == 2){

		//printf("USER: %s\n", j.nombre);	//debug

        if( (strcmp(newJ.nombre, j.nombre) == 0) && (strcmp(newJ.password, j.password) == 0) ){

				fclose(f);
				return 1;
        }
    }

    fclose(f);
    return 0;

}

int GetPosJugador(struct Jugador jugadores[30], int nJugadores, int sd){


	for(int i=0; i<nJugadores; i++){

		if(jugadores[i].sd==sd){

			return i;
		}
	}

	return -1;
}

int getPartidaJugador(struct Partida partidas[15], int nPartidas, int sd){

	for(int i=0; i<nPartidas; i++){

		if((partidas[i].j1 == sd) || (partidas[i].j2==sd)){
			return i;
		}
	}
	return -1;
}

/*-------------------------------------
    TABLERO
---------------------------------------*/

void imprimirTablero(char tablero[10][10]) {

	printf("  | A | B | C | D | E | F | G | H | I | J \n");
	printf("------------------------------------------\n");

	for(int i=0; i<10; i++) {

		printf("%i | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c \n", i, tablero[i][0],
																			tablero[i][1],
																			tablero[i][2],
																			tablero[i][3],
																			tablero[i][4],
																			tablero[i][5],
																			tablero[i][6],
																			tablero[i][7],
																			tablero[i][8],
																			tablero[i][9]);
	}

}

void copiaTablero(char tableroDest[10][10], char tableroOrig[10][10]){
	
	for(int i=0; i<10; i++){
		for(int j=0; j<10; j++){
			tableroDest[i][j]=tableroOrig[i][j];
		}
	}
}

void rellenaTablero(char tablero[10][10], struct Barco barcos[5]) {

	srand(time(NULL));

	//Inicializacion del agua
	//printf("Inicializando agua\n");	//debug
	for(int i=0; i<10; i++) {
		
		for(int j=0; j<10; j++){

			tablero[i][j]='A';
		}
	}

	//Inicialización de los barcos
	//printf("Inicializando barcos\n");	//debug
	for(int b=0; b<5; b++){
		for(int p=0; p<4; p++){
			barcos[b].posiciones[p][0]=-1;
			barcos[b].posiciones[p][1]=-1;
		}
	}

	//BARCO DE 4
	int r = rand() % 2;

	//printf("Colocando barco de 4\n");	//debug

	if(r) while(colocaBarcoHoriz(tablero, 4, barcos));
	else while(colocaBarcoVert(tablero, 4, barcos));

	//2 BARCOS DE 3
	r = rand() % 2;

	//printf("Colocando barcos de 3\n");	//debug

	for(int i=1; i<3; i++){

		if(r) while(colocaBarcoHoriz(tablero, 3, barcos+i));
		else while(colocaBarcoVert(tablero, 3, barcos+i));
	}
	
	//2 BARCOS DE 2
	r = rand() % 2;

	//printf("Colocando barcos de 2\n");	//debug

	for(int i=3; i<5; i++){

		if(r) while(colocaBarcoHoriz(tablero, 2, barcos+i));
		else while(colocaBarcoVert(tablero, 2, barcos+i));
	}
}

int checkHundido(struct Barco barcos[5], int row, int col){

	for(int b=0; b<5; b++){

		//debug
		/*printf("Checking barco %i, longitud %i, posiciones-> (%i,%i), (%i,%i), (%i,%i), (%i,%i)\n",
					b, barcos[b].longitud, barcos[b].posiciones[0][0], barcos[b].posiciones[0][1],
											barcos[b].posiciones[1][0], barcos[b].posiciones[1][1],
											barcos[b].posiciones[2][0], barcos[b].posiciones[2][1],
											barcos[b].posiciones[3][0], barcos[b].posiciones[3][1]);*/

		for(int i=0; i<(barcos[b].longitud); i++){

			if((barcos[b].posiciones[i][0]==row) && (barcos[b].posiciones[i][1]==col)){

				if((barcos[b].vida)>=0){
					(barcos[b].vida)--;
				}
				
				return (barcos[b].vida);
			}
		}
	}
	return -1;
}

int checkFin(struct Barco barcos[5]){
	
	for(int b=0; b<5; b++){

		if((barcos[b].vida)>0){

			return 0;	
		}
	}
	return 1;
}

int colocaBarcoHoriz(char tablero[10][10], int longitud, struct Barco * barco){

	srand(time(NULL));

	int row = rand() % 10;
	int col = rand() % (11 - longitud);

	char tableroAux[10][10];

	for(int r=0; r<10; r++){

		for(int c=0; c<10; c++){
			tableroAux[r][c]=tablero[r][c];
		}
	}
	//printf("Colocando barco horizontal inicio (%i,%i)\n", row, col);	//Debug

	
	int cont=0;
	for(int i=0; i<longitud; i++) {

		if(i==0){	//Comprueba la primera posicion del barco

			if(tableroAux[row][col] != 'B' && (col==0 || tableroAux[row][col-1]!='B')){

				//Comprueba arriba y abajo (igual en todas)
				if( ( row==0 || tableroAux[row-1][col]!='B' ) && (row==10 || tableroAux[row+1][col]!='B') ){

					tableroAux[row][col]='B';

					//Guardar coord. barco
					barco->posiciones[cont][0]=row;
					barco->posiciones[cont][1]=col;
					cont++;

					col++;

				}else return 1;
			}else return 1;
		}
		else if(i==longitud-1){	//Comprueba la ultima posicion del barco

			if(tableroAux[row][col] != 'B' && (col==10 || tableroAux[row][col+1]!='B')){

				if( ( row==0 || tableroAux[row-1][col]!='B' ) && (row==10 || tableroAux[row+1][col]!='B') ){

					tableroAux[row][col]='B';

					//Guardar coord. barco
					barco->posiciones[cont][0]=row;
					barco->posiciones[cont][1]=col;
					cont++;

					col++;
				
				}else return 1;
			}else return 1;
		}
		else{	//Comprueba posiciones intermedias

			if(tableroAux[row][col] != 'B'){
				if( ( row==0 || tableroAux[row-1][col]!='B' ) && (row==10 || tableroAux[row+1][col]!='B') ){
					
					tableroAux[row][col]='B';

					//Guardar coord. barco
					barco->posiciones[cont][0]=row;
					barco->posiciones[cont][1]=col;
					cont++;

					col++;
				
				}else return 1;
			}else return 1;
		}
	}

	for(int r=0; r<10; r++){
		for(int c=0; c<10; c++){
			tablero[r][c]=tableroAux[r][c];
			
			/*if(tablero[r][c]=='B'){
				barco->posiciones[cont][0]=r;
				barco->posiciones[cont][1]=c;
				cont++;
			}*/
		}
	}

	barco->longitud=longitud;
	barco->vida=longitud;

	printf("Barco de longitud (%i) y vida (%i), colocado.\n", barco->longitud=longitud, barco->vida=longitud);

	return 0;
}

int colocaBarcoVert(char tablero[10][10], int longitud, struct Barco * barco){

	srand(time(NULL));

	int row = rand() % (11 - longitud);
	int col = rand() % 11;

	char tableroAux[10][10];

	for(int r=0; r<10; r++){
		for(int c=0; c<10; c++){
			tableroAux[r][c]=tablero[r][c];
		}
	}

	//printf("Colocando barco vertical inicio (%i,%i)\n", row, col);	//Debug

	int cont=0;

	for(int i=0; i<longitud; i++) {

		if(i==0){	//PRIMERA CASILLA DE BARCO

			if(tableroAux[row][col] != 'B' && (row==0 || tableroAux[row-1][col]!='B')){

				if( ( col==0 || tableroAux[row][col-1]!='B' ) && (col==10 || tableroAux[row][col+1]!='B') ){

					tableroAux[row][col]='B';

					//Guardar coord. barco
					barco->posiciones[cont][0]=row;
					barco->posiciones[cont][1]=col;
					cont++;

					row++;
				
				}else return 1;
			}else return 1;
		}
		else if(i==longitud-1){	//ULTIMA CASILLA DE BARCO

			if(tableroAux[row][col] != 'B' && (row==10 || tableroAux[row+1][col]!='B')){

				if( ( col==0 || tableroAux[row][col-1]!='B' ) && (col==10 || tableroAux[row][col+1]!='B') ){

					tableroAux[row][col]='B';

					//Guardar coord. barco
					barco->posiciones[cont][0]=row;
					barco->posiciones[cont][1]=col;
					cont++;

					row++;
				
				}else return 1;
			}else return 1;
		}
		else{

			if(tableroAux[row][col] != 'B'){

				if( ( col==0 || tableroAux[row][col-1]!='B' ) && (col==10 || tableroAux[row][col+1]!='B') ){

					tableroAux[row][col]='B';

					//Guardar coord. barco
					barco->posiciones[cont][0]=row;
					barco->posiciones[cont][1]=col;
					cont++;

					row++;
				
				}else return 1;
			}else return 1;
		}
	}


	for(int r=0; r<10; r++){
		for(int c=0; c<10; c++){
			tablero[r][c]=tableroAux[r][c];
			/*if(tablero[r][c]=='B'){
				barco->posiciones[cont][0]=r;
				barco->posiciones[cont][1]=c;
				cont++;
			}*/
		}
	}

	barco->longitud=longitud;
	barco->vida=longitud;

	return 0;
}

int letterToInt(char c){

	char ABC[10] = "ABCDEFGHIJ";
	char abc[10] = "abcdefghik";
	for(int i=0; i<10; i++){
		if((c==ABC[i]) || (c==abc[i])) return i;
	}
	return -1;
}

