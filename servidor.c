#include "servFunciones.h"


int main ( int argc, char **argv)
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
   	int salida;
   	int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
   	//contadores
    	int i,j,k;
	int recibidos;
   	char identificador[MSG_SIZE];
    
    int on, ret;
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}
    
    // Activaremos una propiedad del socket para permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permite· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}

	
    printf("[*] El juego está esperando jugadores...\n");	//Inicializar los conjuntos fd_set
    	
	FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);
    
   	
    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);
    
    /*--------------------------
        VARIABLES DE JUEGO
    ----------------------------*/
    struct Jugador jugadores[30];
    int nJugadores=0;

    struct Partida partidas[15];
    int nPartidas=0;

    char tableroAux[10][10];

    //Variables auxiliares
        int aux;
        char nombre[100];
        char passwd[100];
        struct Jugador jugadorAux;
        struct Partida partidaAux;

	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1){
            
            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
            
            auxfds = readfds;
            
            salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);
            
            if(salida > 0){
                
                
                for(i=0; i<FD_SETSIZE; i++){
                    
                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds)) {
                        
                        if( i == sd) //NUEVA CONEXION
                        {
                            
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes] = new_sd;
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);
                                
                                    strcpy(buffer, "Bienvenido al juego\n");
                                
                                    send(new_sd,buffer,sizeof(buffer),0);
                                
                                    for(j=0; j<(numClientes-1);j++){
                                    
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "[+] Ok. Usuario conectado <%d>",new_sd);
                                        send(arrayClientes[j],buffer,sizeof(buffer),0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"Demasiados juadores conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                }
                                
                            }
                            
                            
                        }
                        else if (i == 0) //INFO INTRODUCIDA POR TECLADO (LADO DEL SERVIDOR)
                        {
                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);
                            
                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){
                             
                                for (j = 0; j < numClientes; j++){
						            bzero(buffer, sizeof(buffer));
						            strcpy(buffer,"Desconexión servidor\n"); 
                                    send(arrayClientes[j],buffer , sizeof(buffer),0);
                                    close(arrayClientes[j]);
                                    FD_CLR(arrayClientes[j],&readfds);
                                }
                                close(sd);
                                exit(-1);
                            }
                            //DEBUG MODES
                            if(strcmp(buffer,"JUGADORES\n") == 0){
                                for(int j=0; j<nJugadores; j++){
                                    printf("[USUARIO(%i): sd(%i), nombre(%s), password(%s), logged(%i), buscandoPartida(%i)]\n",
                                        j, jugadores[j].sd, jugadores[j].nombre, jugadores[j].password, jugadores[j].logged, jugadores[j].buscandoPartida);
                                }
                            }
                        }
                        else //MENSAJE DE UN CLIENTE
                        {
                            bzero(buffer,sizeof(buffer));
                            
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0){
                                
                                /*-----------------------------------------
                                    OPCIONES
                                ------------------------------------------*/

                                if(strcmp(buffer,"SALIR\n") == 0){
                                    
                                    salirCliente(i,&readfds,&numClientes,arrayClientes);   
                                }
                                if(strncmp(buffer,"USUARIO ", 8) == 0){

                                    sscanf(buffer, "USUARIO %s", nombre);

                                    //printf("Jugador %s recibido!\n", nombre);   //debug

                                    if(JugadorConectado(jugadores, nJugadores, nombre)!=-1){
                                        //AVISAR DE QUE YA ESTÁ CONECTADO
                                        strcpy(buffer, "–Err. Usuario ya conectado\n");
                                        send(i,buffer,sizeof(buffer),0);
                                    }
                                    else{
                                        if(BuscarJugador(nombre)){//+Ok. Usuario correcto
                                            strcpy(buffer, "+Ok. Usuario correcto\n");
                                            send(i,buffer,sizeof(buffer),0);
                                            
                                            //Añadir jugador a jugadores
                                            aux=GetPosJugador(jugadores, nJugadores, i);
                                            if(aux!=-1){

                                                //printf("El nombre ahora deberia ser %s, aux=%i\n", jugadores[aux].nombre, aux);   //debug

                                                strcpy(jugadores[aux].nombre, nombre);
                                                strcpy(jugadores[aux].password, "");
                                                jugadores[aux].sd=i;
                                                jugadores[aux].enPartida=0;
                                                jugadores[aux].logged=0;
                                                jugadores[aux].buscandoPartida=0;
                                            }
                                            else{
                                                strcpy(jugadores[nJugadores].nombre, nombre);
                                                strcpy(jugadores[nJugadores].password, "");
                                                jugadores[nJugadores].sd=i;
                                                jugadores[nJugadores].enPartida=0;
                                                jugadores[nJugadores].logged=0;
                                                jugadores[nJugadores].buscandoPartida=0;

                                                nJugadores++;
                                            }
                                        }
                                        else{//Err. Usuario no registrado.   
                                            strcpy(buffer, "–Err. Usuario incorrecto\n");
                                            send(i,buffer,sizeof(buffer),0);
                                        }
                                    }  
                                }
                                if(strncmp(buffer,"PASSWORD ", 9) == 0){

                                    sscanf(buffer, "PASSWORD %s", passwd);
                                    //printf("Passswd set to %s\n", passwd);  //debug

                                    //printf("Contraseña %s requested! sd(%i)\n", passwd, i);   //debug

                                    //Comprueba que el cliente esté conectado
                                    aux=GetPosJugador(jugadores, nJugadores, i);
                                    if(aux != -1){
                                    
                                        strcpy(jugadores[aux].password, passwd);

                                        //printf("Passswd set to %s (aux=%i)\n", jugadores[aux].password, aux);   //debug

                                        //Comprueba que la contraseña es correcta
                                        if(CheckPassword(jugadores[aux])) {

                                            strcpy(buffer, "+Ok. Usuario validado\n");
                                            send(i,buffer,sizeof(buffer),0);
                                            jugadores[aux].logged=1;
                                        }
                                        else{

                                            strcpy(jugadores[aux].password, "");

                                            strcpy(buffer, "–Err. Error en la validación1\n");
                                            send(i,buffer,sizeof(buffer),0);
                                        }
                                    }
                                    else{

                                        strcpy(buffer, "–Err. Error en la validación2\n");
                                        send(i,buffer,sizeof(buffer),0);
                                    }
                                }
                                if(strncmp(buffer,"REGISTRO ", 9) == 0){
                                    
                                    char nombre[100];
                                    char passwd[100];

                                    sscanf(buffer, "REGISTRO -u %s -p %s", nombre, passwd);

                                    strcpy(jugadorAux.nombre,nombre);
                                    strcpy(jugadorAux.password,passwd);

                                    if(!RellenaFichero(jugadorAux)){

                                        strcpy(buffer, "+Ok. Usuario registrado\n");
                                        send(i,buffer,sizeof(buffer),0);
                                    }
                                    else{
                                        strcpy(buffer, "–Err. Error en el registro\n");
                                        send(i,buffer,sizeof(buffer),0);
                                    }

                                    
                                }
                                if(strncmp(buffer,"DISPARO ", 8) == 0){
                                    
                                    //Disparo
                                }
                                if(strncmp(buffer,"INICIAR-PARTIDA ", 16) == 0){
                                    
                                    aux=GetPosJugador(jugadores, nJugadores, i);

                                    //Buscar si hay alguien esperando
                                    for(int j=0; j<nJugadores; j++){
                                        
                                        if(j!=aux && jugadores[j].buscandoPartida && jugadores[j].logged){
                                            
                                            //HAY JUGADORES ESPERANDO (Inicia la partida)
                                            jugadores[j].buscandoPartida=0;

                                            partidas[nPartidas].id_partida=nPartidas;
                                            partidas[nPartidas].j1=jugadores[j].sd;
                                            partidas[nPartidas].j2=jugadores[aux].sd;
                                            partidas[nPartidas].next=1;
                                            rellenaTablero(partidas[nPartidas].tablero1);
                                            rellenaTablero(partidas[nPartidas].tablero2);

                                            nPartidas++;

                                            strcpy(buffer, "+Ok. Empieza la partida\n");
                                            send(partidaAux.j1,buffer,sizeof(buffer),0);

                                            strcpy(buffer, "+Ok. Empieza la partida\n");
                                            send(partidaAux.j2,buffer,sizeof(buffer),0);

                                            break;
                                        }

                                        if(j==nJugadores-1){

                                            //NO HAY NADIE ESPERANDO
                                            jugadores[aux].buscandoPartida=1;

                                            strcpy(buffer, "+Ok. Esperando jugadores\n");
                                            send(i,buffer,sizeof(buffer),0);
                                        }
                                    }
                                }
                                else{
                                    
                                    //Debug (ALL MESSAGE BROADCAST)
                                    sprintf(identificador,"<%d>: %s",i,buffer);
                                    bzero(buffer,sizeof(buffer));

                                    strcpy(buffer,identificador);

                                    printf("%s\n", buffer);

                                    for(j=0; j<numClientes; j++)
                                        if(arrayClientes[j] != i)
                                            send(arrayClientes[j],buffer,sizeof(buffer),0);

                                    
                                }
                                                                
                                
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;
	
}
