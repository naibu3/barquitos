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

#include "jugador.h"

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
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/

	printf("[*] Iniciando conexión con el servidor\n");

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


	/* ------------------------------------------------------------------
		LOGIN
	-------------------------------------------------------------------*/
	
	struct Jugador a;
    
	if(FD_ISSET(0,&auxfds)){	//USUARIO

		printf("Introduzca nombre de usuario: ");
		fgets(a.nombre,sizeof(a.nombre),stdin);
		send(sd,strcat("USUARIO ", a.nombre),sizeof(a.nombre),0);
    }

	auxfds = readfds;
	salida = select(sd+1,&auxfds,NULL,NULL,NULL);
	
	//Tengo mensaje desde el servidor
	if(FD_ISSET(sd, &auxfds)){
		
		bzero(buffer,sizeof(buffer));
		recv(sd,buffer,sizeof(buffer),0);
		
		printf("\n%s\n",buffer);

		if(strcmp(buffer,"+Ok. Usuario conectado.\n") == 0)
            fin =0;
            
		if(strcmp(buffer,"-Err. Usuario incorrecto\n") == 0){  //REGISTRO

			char opcion[10];
			printf("Desea registrarse(R) o desea termianr la conexion(SALIR)\n");
			fgets(opcion, sizeof(opcion), stdin);
			if(strcmp(opcion,"SALIR\n") == 0){
				close(sd);
    			return 0;
			}

			if(FD_ISSET(0,&auxfds)){

				printf("Introduzca contraseña de : ");
				fgets(a.password,sizeof(a.password),stdin);
				send(sd, strcat(strcat("REGISTRO -u", a.nombre), strcat( " -p ", a.password)), sizeof(a.password),0);
   			}

		}	
	}

	if(FD_ISSET(0,&auxfds)){	//CONTRASEÑA

		printf("Introduzca contraseña de : ");
		fgets(a.password,sizeof(a.password),stdin);
		send(sd,strcat("PASSWORD ", a.password),sizeof(a.password),0);
    }

	auxfds = readfds;
	salida = select(sd+1,&auxfds,NULL,NULL,NULL);
	
	//Tengo mensaje desde el servidor
	if(FD_ISSET(sd, &auxfds)){
		
		bzero(buffer,sizeof(buffer));
		recv(sd,buffer,sizeof(buffer),0);
		
		printf("\n%s\n",buffer);

		if(strcmp(buffer,"+Ok. Usuario validado.\n") == 0)
            fin =0;
            
		if(strcmp(buffer,"-Err. Error en la validación\n") == 0){
			close(sd);
    		return 0;
		}
	}

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





















