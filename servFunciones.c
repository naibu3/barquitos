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

int RellenaFichero(struct Jugador a){

    FILE *f;
    if(f=(fopen("jugadores.txt","a+"))==NULL){

        perror("No se ha añadido correctamente\n");
        return 0;
    }

    struct Jugador b;

    while(fscanf(f,"%s, %s",&b.nombre,&b.password)==2){

        if(strcmp(a.nombre,b.nombre)==0){

            perror("El usuario ya ha sido registrado");
            fclose(f);
            return 0;
        }
        fprintf(f,"%s, %s\n",a.nombre,a.password);
    }

    fclose(f);
    return 1;
}

int BuscarJugador(struct Jugador a){

    FILE *f;
    if(f=(fopen("jugadores.txt","a+"))==NULL){

        perror("No se ha añadido correctamente\n");
        return 0;
    }

    struct Jugador b;

    while(fscanf(f,"%s, %s",&b.nombre,&b.password)==2){

        if(strcmp(&a.nombre,&b.nombre)==0){

            fclose(f);
            return 0;
        }
        
    }

    fclose(f);
    return 1;

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

void rellenaTablero(char tablero[10][10]) {

	srand(time(NULL));

	for(int i=0; i<10; i++) {
		
		for(int j=0; j<10; j++){

			tablero[i][j]='A';
		}
	}

	//BARCO DE 4
	int r = rand() % 2;

	if(r) while(colocaBarcoHoriz(tablero, 4));
	else while(colocaBarcoVert(tablero, 4));

	//2 BARCOS DE 3
	r = rand() % 2;

	for(int i=0; i<2; i++){

		if(r) while(colocaBarcoHoriz(tablero, 3));
		else while(colocaBarcoVert(tablero, 3));
	}
	
	//2 BARCOS DE 2
	r = rand() % 2;

	for(int i=0; i<2; i++){

		if(r) while(colocaBarcoHoriz(tablero, 2));
		else while(colocaBarcoVert(tablero, 2));
	}
}

int colocaBarcoHoriz(char tablero[10][10], int longitud){

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

	

	for(int i=0; i<longitud; i++) {

		if(i==0){	//Comprueba la primera posicion del barco

			if(tableroAux[row][col] != 'B' && (col==0 || tableroAux[row][col-1]!='B')){

				//Comprueba arriba y abajo (igual en todas)
				if( ( row==0 || tableroAux[row-1][col]!='B' ) && (row==10 || tableroAux[row+1][col]!='B') ){

					tableroAux[row][col]='B';
					col++;

				}else return 1;
				//printf("1\n");	//Debug
			}else return 1;
			//printf("2\n");	//Debug

		}
		else if(i==longitud-1){	//Comprueba la ultima posicion del barco

			if(tableroAux[row][col] != 'B' && (col==10 || tableroAux[row][col+1]!='B')){

				if( ( row==0 || tableroAux[row-1][col]!='B' ) && (row==10 || tableroAux[row+1][col]!='B') ){

					tableroAux[row][col]='B';
					col++;
				
				}else return 1;
				//printf("3\n");	//Debug
			}else return 1;
			//printf("4\n");	//Debug
		}
		else{	//Comprueba posiciones intermedias

			//printf("7\n");	//Debug
			if(tableroAux[row][col] != 'B'){
				//printf("8\n");	//Debug
				if( ( row==0 || tableroAux[row-1][col]!='B' ) && (row==10 || tableroAux[row+1][col]!='B') ){
					
					//printf("9\n");	//Debug
					tableroAux[row][col]='B';
					col++;
				
				}else return 1;
				//printf("5\n");	//Debug
			}else return 1;
			//printf("6\n");	//Debug
		}
	}

	for(int r=0; r<10; r++){
		for(int c=0; c<10; c++){
			tablero[r][c]=tableroAux[r][c];
		}
	}

	return 0;
}

int colocaBarcoVert(char tablero[10][10], int longitud){

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

	for(int i=0; i<longitud; i++) {

		if(i==0){

			if(tableroAux[row][col] != 'B' && (row==0 || tableroAux[row-1][col]!='B')){

				if( ( col==0 || tableroAux[row][col-1]!='B' ) && (col==10 || tableroAux[row][col+1]!='B') ){

					tableroAux[row][col]='B';
					row++;
				
				}else return 1;
			}else return 1;
		}
		else if(i==longitud){

			if(tableroAux[row][col] != 'B' && (row==10 || tableroAux[row+1][col]!='B')){

				if( ( col==0 || tableroAux[row][col-1]!='B' ) && (col==10 || tableroAux[row][col+1]!='B') ){

					tableroAux[row][col]='B';
					row++;
				
				}else return 1;
			}else return 1;
		}
		else{

			if(tableroAux[row][col] != 'B'){

				if( ( col==0 || tableroAux[row][col-1]!='B' ) && (col==10 || tableroAux[row][col+1]!='B') ){

					tableroAux[row][col]='B';
					row++;
				
				}else return 1;
			}else return 1;
		}
	}

	for(int r=0; r<10; r++){
		for(int c=0; c<10; c++){
			tablero[r][c]=tableroAux[r][c];
		}
	}

	return 0;
}