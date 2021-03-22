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
#define MAXBUFLEN 1500
#define MAX_NAME 100
#define MAX_DATA 1000
int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	char fname[200];
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
		printf("client>\n");
		//get command
		fgets(fname, 200, stdin);
		fname[strcspn(fname,"\n")]=0;
		j=0; cnt=0;
		memset(splitStrings,0,sizeof(splitStrings));
		for(i=0;i<=(strlen(fname));i++)
		{
			if(fname[i]==' '||fname[i]=='\0'){
	           	splitStrings[cnt][j]='\0';
	        	cnt++;  //for next word
	    		j=0;    //for next word, init index to 0
	    	}
	       	else{
	        	splitStrings[cnt][j]=fname[i];
	        	j++;
	        }

	    }
		// printf(splitStrings[0]);
		// printf(splitStrings[1]);
		// printf(splitStrings[2]);
		// printf(splitStrings[3]);
		// printf(splitStrings[4]);
		//handling command
		//printf("Handling command");
		if(strcmp(splitStrings[0],"/login")==0){
			printf("Sending login request\n");

			memset(&hints, 0, sizeof hints);
			if ((rv = getaddrinfo(splitStrings[3], splitStrings[4], &hints, &servinfo)) != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
			//printf("Got addrinfo");
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC; // set to unspecified protocol
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
			//Make Struct
			struct message msg;
			msg.type=0;
			//msg.source=splitStrings[1];
			//msg.data=splitStrings[2];
			msg.size=sizeof(msg.data);
			//Turn struct to string to send
			char msgStr[1500]="";
			char numtostr[100];
			strcat(msgStr,"LOGIN:");
			sprintf(numtostr,"%d", msg.size);
			strcat(msgStr,numtostr);
			strcat(msgStr,":");
			strcat(msgStr,splitStrings[1]);
			strcat(msgStr,":");
			strcat(msgStr,splitStrings[2]);
			//send login req
			printf("Sending...\n");
			if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
				perror("send");
			}
			memset(buf,0,MAXBUFLEN);
			//look for resp
			if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
				perror("recv");
				exit(1);
			}
			//process resp
			buf[numbytes]='\0';
			if(strcmp(buf,"LO_ACK")==0){
				printf("Successful Login\n");
			}
			if(strcmp(buf,"LO_NAK")==0){
				printf("Unsuccessful Login\n");
				continue;
			}
			//Enter server
			for(;;){
				char prompt[100]="client/";
				strcat(prompt,splitStrings[3]);
				strcat(prompt,":");
				strcat(prompt,splitStrings[4]);
				strcat(prompt,">");
				printf(prompt);
				//get command
				fgets(fname, 100, stdin);
				fname[strcspn(fname,"\n")]=0;

				j=0; cnt=0;
				memset(splitStrings,0,sizeof(splitStrings));
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
				if(strcmp(splitStrings[0],"/logout")){close(sockfd);break;}
			}


			


			
		}
		
		if(strcmp(splitStrings[0],"/quit")){break;}
		


	}


	
	
	close(sockfd);

	return 0;
}
