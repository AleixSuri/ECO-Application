/**************************************************************************/
/*                                                                        */
/* L'aplicació ECO amb sockets TCP/IP                                     */
/* Fitxer cliECO.c que implementa el client ECO sobre la capa de          */
/* transport TCP (fent crides a la "nova" interfície de la capa de        */
/* transport o "nova" interfície de sockets).                             */
/*                                                                        */
/* Autors: Arnau Herrera i Aleix Suriñach                                 */
/* Data: octubre 2024                                                     */
/*                                                                        */
/**************************************************************************/

#include <stdio.h>
#include "p1-tTCP.h"
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>


/* Crea un socket TCP per a un client i l'assigna a “scon”.                 */
/* La funció inicialitza el socket i realitza les comprovacions necessàries */
/* per assegurar-se que s'ha creat correctament. Si es produeix un error,   */
/* es retorna el codi d'error a “CodiRes”.                                  */
/*                                                                          */
/* Retorna:                                                                 */
/*  0 si el socket s'ha creat correctament;                                 */
/* -1 si hi ha hagut un error en la creació del socket.                     */
int CrearSocketClient(int *scon, int *CodiRes);

/* Llegeix l'adreça IP del servidor a partir de l'entrada de l'usuari i la  */
/* guarda a “iprem”. Aquesta funció valida l'adreça proporcionada i retorna */
/* un codi d'error a “CodiRes” si l'entrada no és vàlida.                   */
/*                                                                          */
/* Retorna:                                                                 */
/*  0 si l'adreça IP s'ha llegit i validat correctament;                    */
/* -1 si s'ha produït un error en llegir o validar l'adreça IP.             */
int LlegirIPServidor(char *iprem, int *CodiRes);

/* Funció per llegir el número de port del servidor a partir de l'entrada   */
/* de l'usuari i el guarda a “portrem”. Aquesta funció valida el port       */
/* proporcionat i retorna un codi d'error a “CodiRes” si l'entrada no és    */
/* vàlida.                                                                  */
/*                                                                          */
/* Retorna:                                                                 */
/*  0 si el port s'ha llegit i validat correctament;                        */
/* -1 si s'ha produït un error en llegir o validar el port.                 */
int LlegirPortServidor(int *portrem, int *CodiRes);

/* Funció per mostrar les adreces IP i ports associats al socket “scon”.     */
/* Aquesta funció obté i imprimeix la informació de la connexió actual,      */
/* incloent tant l'adreça local com l'adreça remota. Retorna un codi d'error */
/* a “CodiRes” si es produeix algun problema durant l'obtenció de la         */
/* informació.                                                               */
/*                                                                           */
/* Retorna:                                                                  */
/*  0 si les adreces s'han mostrat correctament;                             */
/* -1 si s'ha produït un error en obtenir o mostrar les adreces i ports.     */
int MostrarAdreces(int scon, int *CodiRes);

/* Funció per gestionar l'enviament i la recepció de missatges a través      */
/* del socket “scon”. Aquesta funció s'encarrega de gestionar el flux de     */
/* comunicació, enviant un missatge al servidor i esperant la resposta.      */
/* Retorna un codi d'error a “CodiRes” si es produeix un problema durant     */
/* el procés.                                                                */
/*                                                                           */
/* Retorna:                                                                  */
/*  0 si l'enviament i la recepció s'han realitzat correctament;             */
/* -1 si s'ha produït un error en l'enviament o la recepció del missatge.    */
int EnviarIRebreMissatge(int scon, int *CodiRes);

/* Funció per tancar el socket identificat per “scon” i gestionar errors.    */
/* Aquesta funció es preocupa de tancar la connexió de manera segura,        */
/* alliberant els recursos associats al socket. Retorna un codi d'error a    */
/* “CodiRes” si es produeix un problema durant el tancament del socket.      */
/*                                                                           */
/* Retorna:                                                                  */
/*  0 si el socket s'ha tancat correctament;                                 */
/* -1 si s'ha produït un error en tancar el socket.                          */
int TancarSocket(int scon, int *CodiRes);



/* Funció principal */
int main(int argc, char *argv[]) 
{
    int scon;
    char continuar = 'S';
    int CodiRes;

    while(continuar != 'N') 
	{
        char iprem[16];
        int portrem;

        // Crea socket client
        CrearSocketClient(&scon, &CodiRes);

        // Llegeix IP i port del servidor
        LlegirIPServidor(iprem, &CodiRes);
		LlegirPortServidor(&portrem, &CodiRes);

        // Sol·licita connexió
        if(TCP_DemanaConnexio(scon, iprem, portrem) == -1) 
		{
            printf("\nTCP_DemanaConnexio(): %s\n", T_ObteTextRes(&CodiRes));
            exit(-1);
        }

        // Mostra adreces de client i servidor
        MostrarAdreces(scon, &CodiRes);

        // Enviar i rebre missatges
        EnviarIRebreMissatge(scon, &CodiRes);

        char buffer[200];
        int bytes_llegits;
		//Continuar?
        printf("\nVols continuar? (S/N) \n");
        if((bytes_llegits = read(0, buffer, sizeof(buffer))) == -1) 
		{
            printf("\nread(): %s\n", T_ObteTextRes(&CodiRes));
            close(scon);
			exit(-1);
        }
        buffer[bytes_llegits-1] = '\0';
        continuar = buffer[0];

        // Tanca el socket
        TancarSocket(scon, &CodiRes);
    }
}




int CrearSocketClient(int *scon, int *CodiRes) 
{
    if((*scon = TCP_CreaSockClient("0.0.0.0", 0)) == -1) 
	{
        printf("\nTCP_CreaSockClient(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }
    return 0;
}

int LlegirIPServidor(char *iprem, int *CodiRes) 
{
    char buffer[200];
    int bytes_llegits;
    
    printf("\nEntrar IP del servidor: \n");
    if((bytes_llegits = read(0, buffer, sizeof(buffer))) == -1) 
	{
        printf("\nread(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }
    buffer[bytes_llegits-1] = '\0';
    strcpy(iprem, buffer);
    
    return 0;
}

int LlegirPortServidor(int *portrem, int *CodiRes) 
{
    char buffer[200];
    int bytes_llegits;

    printf("\nEntrar port del servidor: \n");
    if((bytes_llegits = read(0, buffer, sizeof(buffer))) == -1) 
	{
        printf("\nread(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }
    buffer[bytes_llegits-1] = '\0';
    *portrem = atoi(buffer);

    return 0;
}

int MostrarAdreces(int scon, int *CodiRes) 
{
    char IPloc[16], IPrem[16];
    int portTCPloc, portTCPrem;

    if(TCP_TrobaAdrSockLoc(scon, IPloc, &portTCPloc) == -1) 
	{
        printf("\nTCP_TrobaAdrSockLoc(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }

    if(TCP_TrobaAdrSockRem(scon, IPrem, &portTCPrem) == -1) 
	{
        printf("\nTCP_TrobaAdrSockRem(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }

    printf("\n----------------------------------------------------\n");
    printf("Adreces socket client: %s:%d\n", IPloc, portTCPloc);
    printf("\nAdreces socket servidor: %s:%d\n", IPrem, portTCPrem);
    printf("----------------------------------------------------\n");

    return 0;
}

int EnviarIRebreMissatge(int scon, int *CodiRes) 
{
    char SeqBytes[200];
    int bytes_llegits;

    // Entra missatge
    printf("\nEntra una línia (entra el caràcter '#', per acabar): \n");
    if((bytes_llegits = read(0, SeqBytes, sizeof(SeqBytes))) == -1) 
	{
        printf("\nread(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }

    while(SeqBytes[0] != '#') // Mentre no sigui '#'
	{  
        int bytes_enviats, bytes_rebuts;
        char SeqBytes_buit[200];

        // Envia missatge
        if((bytes_enviats = TCP_Envia(scon, SeqBytes, bytes_llegits)) == -1) 
		{
            printf("\nTCP_Envia(): %s\n", T_ObteTextRes(CodiRes));
            exit(-1);
        }
        printf("\nBytes enviats: %d \n", bytes_enviats);

        // Rep missatge de servidor
        if((bytes_rebuts = TCP_Rep(scon, SeqBytes_buit, sizeof(SeqBytes_buit))) == 0) 
		{
            printf("\nEl servidor s'ha desconnectat. \n");
            return -1;
        } else if(bytes_rebuts == -1) 
		{
            printf("\nTCP_Rep(): %s\n", T_ObteTextRes(CodiRes));
            exit(-1);
        }

        // Mostra el missatge rebut
        printf("\nMissatge eco: \n");
        if(write(1, SeqBytes_buit, bytes_rebuts) == -1) 
		{
            printf("\nwrite(): %s\n", T_ObteTextRes(CodiRes));
            exit(-1);
        }

        // Torna a demanar el següent missatge
        printf("\nEntra una línia (entra el caràcter '#', per acabar): \n");
        if((bytes_llegits = read(0, SeqBytes, sizeof(SeqBytes))) == -1) 
		{
            printf("\nread(): %s\n", T_ObteTextRes(CodiRes));
            exit(-1);
        }
    }

    return 0;
}

int TancarSocket(int scon, int *CodiRes) 
{
    int res;
	if(res=TCP_TancaSock(scon) == -1) 
	{
        printf("\nTCP_TancaSock(): %s\n", T_ObteTextRes(CodiRes));
        exit(-1);
    }
    return res;
}