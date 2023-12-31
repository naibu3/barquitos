#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "servFunciones.h"

//#include "jugador.h"

int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;
	
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

   
    
	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2065);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/

	printf("[*] Iniciando conexión con el servidor\n\n");

	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

	/*-----------------------------------------
		VARIABLES DE JUEGO
	-----------------------------------------*/

	char tablero[10][10];
	char ccol;
	int row, col;

	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){
            
            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);
            
            printf("\n%s\n",buffer);
            
            if(strcmp(buffer,"Demasiados clientes conectados\n") == 0)
                fin =1;
            
            if(strcmp(buffer,"Desconexión servidor\n") == 0)
                fin =1;

			//COMPROBACIONES

			if(strncmp(buffer,"+Ok. Empieza la partida", 23) == 0){
                //PONER EL TABERO A CERO (*)
				for(int r=0; r<10; r++){
					for(int c=0; c<10; c++){
						tablero[r][c]='*';
					}
				}
				imprimirTablero(tablero);
            }

			if(strncmp(buffer,"+Ok. TOCADO: ", 13) == 0){
                //TOCADO
				sscanf(buffer, "+Ok. TOCADO: %c,%i", &ccol, &row);
                col=letterToInt(ccol);

				tablero[row][col]='B';

				imprimirTablero(tablero);
            }
			if(strncmp(buffer,"+Ok. HUNDIDO: ", 14) == 0){
                //HUNDIDO
				sscanf(buffer, "+Ok. HUNDIDO: %c,%i", &ccol, &row);
                col=letterToInt(ccol);

				tablero[row][col]='B';

				imprimirTablero(tablero);
            }
			if(strncmp(buffer,"+Ok. AGUA: ", 11) == 0){
                //AGUA
				sscanf(buffer, "+Ok. AGUA: %c,%i", &ccol, &row);
                col=letterToInt(ccol);

				tablero[row][col]='A';

				imprimirTablero(tablero);
            }
            
        }
        else
        {
            
            //He introducido información por teclado
            if(FD_ISSET(0,&auxfds)){
                bzero(buffer,sizeof(buffer));
                
                fgets(buffer,sizeof(buffer),stdin); 		//COMANDO
                
                if(strcmp(buffer,"SALIR\n") == 0){
                        fin = 1;
                }

                send(sd,buffer,sizeof(buffer),0); 
            }
        }

    }while(fin == 0);
		
    close(sd);

    return 0;
		
}

















