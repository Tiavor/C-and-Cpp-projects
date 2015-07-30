/* A simple server in the internet domain using TCP.
myServer.c
D. Thiebaut
Adapted from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
The port number used in 51717.
This code is compiled and run on the Raspberry as follows:
   
    g++ -o myServer myServer.c
    ./myServer

The server waits for a connection request from a client.
The server assumes the client will send positive integers, which it sends back multiplied by 2.
If the server receives -1 it closes the socket with the client.
If the server receives -2, it exits.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

void error(char* msg ) {
  printf(msg);
}

int func( int a ) {
   return 2 * a;
}


int main(int argc, char *argv[]) {
     int sockfd, newsockfd, portno = 51717, clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     char data[4];
	 int datavalue=-2;
	 bool ret=false;

     printf( "using port #%d\n", portno );
   
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
         error( "ERROR opening socket" );
     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons( portno );
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
       error( "ERROR on binding" );
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
 
     //--- infinite wait on a connection ---
     while ( 1 ) {
        printf( "waiting for new client...\n" );
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 ){
            error( "ERROR on accept\n");
			break;
		}
        printf( "opened new communication with client\n" );
        while ( 1 ) {
			ret = false;
            //---- wait for a number from client ---
			if ((n=recv(newsockfd, data , 4 , 0))<0){
				error("ERROR reading from socket\n");
				break;
			}
			if (n>4){
				error( "wrong byte size recieved\n" );
				ret = true;
			}
			
			datavalue = data[0] + ((int)data[1] << 8) + ((int)data[2] << 16) + ((int)data[3] << 24);
			if (datavalue == 0){
				error("data runs asyc\n");
				ret=true;
			}
            printf( "got %d\n", datavalue );
			
			if(ret){
				int paramInt=-1; int i = 32;
				while ( i > 0 ) { data[i/8] = paramInt >> i; i -= 8; }
			}
            if (datavalue <0||ret){
				printf("restarting connection\n");
				if (!ret){break;}
			}
            //--- send new data back ---
			if (ret){
				datavalue = data[0] + ((int)data[1] << 8) + ((int)data[2] << 16) + ((int)data[3] << 24);
			}
            printf( "sending back %d\n", datavalue );
			if ( (n = write( newsockfd, data, 4 ) ) < 0 ){
				error( "ERROR writing to socket\n" );
				break;
			}
			if (ret){break;}
        }
        close( newsockfd );

        //--- if -2 sent by client, we can quit ---
        if ( datavalue == -2 ){
			printf("shutting down connection\n");
			break;
		}
    }
    return 0;
}