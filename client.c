/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>
#include <libgen.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
//#define SERVERPORT "4950"	// the port users will be connecting to
#define MAXBUFLEN 100
#define MAX_NAME 100
#define MAX_DATA 1000
int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	char fname[100];
	char splitStrings[5][50];
	int i, j, cnt;
	char buf[MAXBUFLEN];
	char filepath[100];
	FILE *file;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	struct message {
		unsigned int type;
		unsigned int size;
		unsigned char source[MAX_NAME];
		unsigned char data[MAX_DATA]; 
	};

	for(;;){
		printf("client>");
		fgets(fname, 100, stdin);
		fname[strcspn(fname,"\n")]=0;

		j=0; cnt=0;
		for(i=0;i<=(strlen(fname));i++)
		{
			if(fname[i]==' '||fname[i]=='\0')
	        	{
	            		splitStrings[cnt][j]='\0';
	            		cnt++;  //for next word
	            		j=0;    //for next word, init index to 0
		    	}
	        	else
	        	{
	            		splitStrings[cnt][j]=fname[i];
	            		j++;
	        	}

	        }
		if(strcmp(splitStrings[0],"/login")){
			if ((rv = getaddrinfo(splitStrings[3], splitStrings[4], &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}

			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC; // set to AF_INET to use IPv4
			hints.ai_socktype = SOCK_STREAM;
			for(p = servinfo; p != NULL; p = p->ai_next) {
				if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
					perror("client: socket");
					continue;
				}
				if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {            
					close(sockfd);
					perror("client: connect");
					continue;
				}

				break;
			}
			if (p == NULL) {
				fprintf(stderr, "talker: failed to create socket\n");
				return 2;
			}

			
		}

		if(strcmp(splitStrings[0],"/quit")){break;}
		


	}


	
	
	close(sockfd);

	return 0;
}
