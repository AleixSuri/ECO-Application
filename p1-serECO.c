/**************************************************************************/
/*                                                                        */
/* L'aplicació ECO amb sockets TCP/IP                                     */
/* Fitxer serECO.c que implementa el servidor ECO sobre la capa de        */
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

/*Aquestes funcions retornen totes 0, per assimilar les accions void en C++*/

/* Llegeix un port TCP des de l'entrada estàndard i l'emmagatzema a       */
/* la variable apuntada per "port". El port ha de ser un valor entre      */
/* 1024 i 49151, o pot ser 0 si es vol que el sistema assigni un port.    */
/*                                                                        */
/* Paràmetres:                                                            */
/*  "port" és un punter a un enter on s'emmagatzemarà el valor llegit.    */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
int llegirPortTCP(int *port);

/* Crea un socket servidor TCP, vinculat a l'adreça IP local i al port    */
/* especificat per "port". Si "port" és 0, el sistema assigna un port     */
/* lliure automàticament.                                                 */
/*                                                                        */
/* Paràmetres:                                                            */
/*  "port" és el número de port TCP al qual es vincula el socket. Ha de   */
/*  ser un valor entre 1024 i 49151, o 0 per a l'assignació automàtica.   */
/*                                                                        */
/* Retorna:                                                               */
/*  El descriptor del socket si tot va bé;                                */
/* -1 si hi ha un error en la creació o configuració del socket.          */
int crearSocketServidor(int port);

/* Accepta una connexió entrant en el socket servidor d'identificador     */
/* "sesc" i omple la informació de la connexió acceptada.                 */
/*                                                                        */
/* Paràmetres:                                                            */
/*  "sesc" és el descriptor del socket servidor que espera connexions.    */
/*  "scon" és un punter on es guardarà el descriptor del socket           */
/*  associat a la connexió acceptada.                                     */
/*  "IPrem" és un "string" de C (vector de chars imprimibles acabat en    */
/*  '\0') d'una longitud màxima de 16 chars (incloent '\0'), on es        */
/*  guardarà l'adreça IP del client remot.                                */
/*  "portTCPrem" és un punter a un enter on es guardarà el número de port */
/*  TCP del client remot.                                                 */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé i s'accepta una connexió;                              */
int acceptarConnexio(int sesc, int *scon, char *IPrem, int *portTCPrem);

/* Mostra les adreces IP i els números de port del socket local i         */
/* remot associats al descriptor de socket "scon".                        */
/*                                                                        */
/* Paràmetres:                                                            */
/*  "scon" és el descriptor del socket TCP ja connectat.                  */
/*                                                                        */
/* La funció obté i imprimeix l'adreça IP i el port TCP del socket local  */
/* i del socket remot (amb qui està connectat).                           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si es mostren correctament les adreces;                             */
int mostrarAdreces(int scon);

/* Gestió d'una connexió establerta a través del socket identificat per     */
/* “scon”. Aquesta funció s'encarrega de tractar la comunicació amb el      */
/* client o servidor connectat, processant les dades enviades i responent   */
/* adequadament.                                                            */
/*                                                                          */
/* Retorna:                                                                 */
/*  0 si la connexió s'ha gestionat correctament;                           */
int tractarConnexio(int scon);



/* Funció principal */
int main(int argc, char *argv[]) 
{
    int sesc, scon, portTCPrem;
    char IPrem[16];
    
    llegirPortTCP(&portTCPrem);
    
    sesc = crearSocketServidor(portTCPrem);
    
    // Bucle principal per acceptar connexions i tractar-les
    while(1) 
	{
        acceptarConnexio(sesc, &scon, IPrem, &portTCPrem);
        
        tractarConnexio(scon);
        
        printf("\nClient desconnectat, esperant una nova connexió...\n");
    }
}



int llegirPortTCP(int *port) 
{
    char buffer[200];
	int CodiRes;
    int bytes_llegits;
    printf("\nDigue'm el port TCP: (0 o valor entre 1024 i 49151)\n");
    if((bytes_llegits = read(0, buffer, sizeof(buffer))) == -1) 
	{
        printf("\nread(): %s\n", T_ObteTextRes(&CodiRes));
        exit(-1);
    }
    buffer[bytes_llegits - 1] = '\0';
    *port = atoi(buffer);
	
	return 0;
}

int crearSocketServidor(int port) 
{
    int sesc;
    int CodiRes;
    
    if((sesc = TCP_CreaSockServidor("0.0.0.0", port)) == -1) 
	{
        printf("\nTCP_CreaSockServidor(): %s\n", T_ObteTextRes(&CodiRes));
        exit(-1);
    }
	
    return sesc;
}

int acceptarConnexio(int sesc, int *scon, char *IPrem, int *portTCPrem) 
{
    int CodiRes;
    
    if((*scon = TCP_AcceptaConnexio(sesc, IPrem, portTCPrem)) == -1) 
	{
        printf("\nTCP_AcceptaConnexio(): %s\n", T_ObteTextRes(&CodiRes));
        TCP_TancaSock(*scon);
        exit(-1);
    }
	
	return 0;
}

int mostrarAdreces(int scon) 
{
    char IPloc[16], IPrem[16];
    int portTCPloc, portTCPrem;
    int CodiRes;
    
    if(TCP_TrobaAdrSockLoc(scon, IPloc, &portTCPloc) == -1) 
	{
        printf("\nTCP_TrobaAdrSockLoc(): %s\n", T_ObteTextRes(&CodiRes));
        exit(-1);
    }
    
    if(TCP_TrobaAdrSockRem(scon, IPrem, &portTCPrem) == -1) 
	{
        printf("\nTCP_TrobaAdrSockRem(): %s\n", T_ObteTextRes(&CodiRes));
        exit(-1);
    }
    
    printf("\n----------------------------------------------------\n");
    printf("Adreces socket servidor: %s:%d\n", IPloc, portTCPloc);
    printf("Adreces socket client: %s:%d\n", IPrem, portTCPrem);
    printf("----------------------------------------------------\n");
	
	return 0;
}

int tractarConnexio(int scon) 
{
    char SeqBytes[200];
    int LongSeqBytes = sizeof(SeqBytes);
    int bytes_rebuts;
    int CodiRes;
    
    mostrarAdreces(scon);
    
    // Bucle per rebre i respondre els missatges del client
    while((bytes_rebuts = TCP_Rep(scon, SeqBytes, LongSeqBytes)) > 0) 
	{
        // Mostra el missatge rebut
        printf("\nMissatge rebut: \n");
        if(write(1, SeqBytes, bytes_rebuts) == -1) 
		{
            printf("\nwrite(): %s\n", T_ObteTextRes(&CodiRes));
            TCP_TancaSock(scon);
            exit(-1);
        }
        
        printf("\nNombre de bytes rebuts: %d\n", bytes_rebuts);
        
        // Envia el missatge de tornada
        if(TCP_Envia(scon, SeqBytes, bytes_rebuts) == -1) 
		{
            printf("\nTCP_Envia(): %s\n", T_ObteTextRes(&CodiRes));
            TCP_TancaSock(scon);
            exit(-1);
        }
    }
    
    if(bytes_rebuts == -1) 
	{
        printf("\nTCP_Rep(): %s\n", T_ObteTextRes(&CodiRes));
        TCP_TancaSock(scon);
        exit(-1);
    }
    
    // Tancar el socket després de la comunicació
    TCP_TancaSock(scon);
	
	return 0;
}