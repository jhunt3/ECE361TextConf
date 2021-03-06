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
#define MAXBUFLEN 2000
#define MAX_NAME 100
#define MAX_DATA 2000
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
			char addrport[20];
			char clientID[20];
			strcpy(clientID,splitStrings[1]);
			strcpy(addrport,splitStrings[3]);
			strcat(addrport,":");
			strcat(addrport,splitStrings[4]);
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
			char prompt1[100];
			memset(prompt1,0,100);
			strcpy(prompt1,"client/");
			strcat(prompt1,splitStrings[3]);
			strcat(prompt1,":");
			strcat(prompt1,splitStrings[4]);
			strcat(prompt1,">");
			for(;;){
				memset(splitStrings,0,sizeof(splitStrings));
				printf(prompt1);
				//get command
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
				
				if(strcmp(splitStrings[0],"/createsession")==0){
					memset(msgStr,0,1500);
					strcat(msgStr,"NEW_SESS:");
					sprintf(numtostr,"%d", sizeof(splitStrings[1]));
					strcat(msgStr,numtostr);
					strcat(msgStr,":");
					strcat(msgStr,clientID);
					strcat(msgStr,":");
					strcat(msgStr,splitStrings[1]);
					if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
						perror("send");
					}
					memset(buf,0,MAXBUFLEN);
					//look for resp
					if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
						perror("recv");
						exit(1);
					}
					buf[numbytes]='\0';
					if(strcmp(buf,"NS_ACK")==0){
						printf("Session succesfully created\n");
					}
					

				}
				if(strcmp(splitStrings[0],"/list")==0){
					//printf("getting list");
					memset(msgStr,0,1500);
					strcat(msgStr,"QUERY:");
					sprintf(numtostr,"%d", 0);
					strcat(msgStr,numtostr);
					strcat(msgStr,":");
					strcat(msgStr,clientID);
					strcat(msgStr,":");

					if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
						perror("send");
					}
					memset(buf,0,MAXBUFLEN);
					//look for resp
					if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
						perror("recv");
						exit(1);
					}
					buf[numbytes]='\0';
					//if(strcmp(buf,"QU_ACK")==0){
					printf("%s\n",buf);
					//}
					

				}				

				if(strcmp(splitStrings[0],"/joinsession")==0){
					memset(msgStr,0,1500);
					strcat(msgStr,"JOIN:");
					sprintf(numtostr,"%d", sizeof(splitStrings[1]));
					strcat(msgStr,numtostr);
					strcat(msgStr,":");
					strcat(msgStr,clientID);
					strcat(msgStr,":");
					strcat(msgStr,splitStrings[1]);
					if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
						perror("send");
					}
					memset(buf,0,MAXBUFLEN);
					//look for resp
					if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
						perror("recv");
						exit(1);
					}
					buf[numbytes]='\0';
					if(strcmp(buf,"JN_ACK")==0){
						printf("Session succesfully joined\n");
					}
					
					char prompt[100];
					memset(prompt,0,100);
					strcat(prompt,"client/");
					strcat(prompt,addrport);
					strcat(prompt,"/");
					strcat(prompt,splitStrings[1]);
					char currsess[50];
					memset(currsess,0,50);
					strcpy(currsess,splitStrings[1]);
					strcat(prompt,">");
					//printf(prompt);
					fd_set read_fds;
					fd_set master;
					int stdin_fd=fileno(stdin);
					//printf("stdin fileno: %d\n",stdin_fd);
					int fdmax; 
					FD_ZERO(&read_fds);
					FD_SET(sockfd,&read_fds);
					FD_SET(stdin_fd,&read_fds);
					fdmax=sockfd;
					master=read_fds;
					if(stdin_fd>fdmax){fdmax=stdin_fd;}
					//fflush( stdout );
					//printf("Sockfd: %d\n",sockfd);
					//printf("\n");
					//printf("%s",prompt);
					//printf("Hello");
					//printf("Sockfd: %d\n",sockfd);
					//for(int p=0; p<5; p++){
					for(;;){	
						//printf("Sockfd1: %d\n",sockfd);
						//printf("entering room...\n");
						//printf("%d\n",);
						read_fds=master;
						printf("%s\n",prompt);
						// memset(buf,0,MAXBUFLEN);
						
						// if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
						// 	perror("recv");
						// 	exit(1);
						// }
						// printf(buf);

						//printf("Sockfd2: %d\n",sockfd);
						if (select(sockfd+1, &read_fds, NULL, NULL, NULL) == -1) {            
            				perror("select");            
            				exit(4);  
						}
						//printf("\n");
						//printf("Found something to read\n");
						//printf("Sockfd3: %d\n",sockfd);
						// //trun through the existing connections looking for data to read
						// for(i = 0; i <= fdmax; i++) {
						// 	if (FD_ISSET(i, &read_fds)) {
						// 		if(i==0){
						// 			memset(fname,0,100);
						// 			fgets(fname, 100, stdin);

						// 		}
						// 	}
						// }

						bool leave=false;
        				for(i = 0; i <= fdmax; i++) {         
            				if (FD_ISSET(i, &read_fds)) { // we got one!! 
								if(i==sockfd){
									printf("Socket to read\n");
									memset(buf,0,MAXBUFLEN);                   
									if ((numbytes = recv(i, buf, MAXBUFLEN, 0)) <= 0) {
										// got error or connection closed by client                        
										if (numbytes == 0) {                            
											// connection closed                            
											printf("selectserver: socket %d hung up\n", i);                        
										} else {                            
											perror("recv");                        
										}                        
										close(i); // bye!                        
										FD_CLR(i, &read_fds); // remove from master set                    
									}else{
										//printf("selectserver: socket %d sent message\n", i);
										printf(buf);
										//printf("\n");
										

										char type[20];
										char value[100];
										int size;
										char source[50];
										char data[1000];
										int cnt=0;
										int arg=1;
										for(int k=0;k<500;k++){
											//printf("%d",k);
											if(buf[k]==':'){
												//printf(value);
												//printf("\n");
												if(arg==1){
													strcpy(type,value);
												}else if(arg==2){
													size=atoi(value);
												}else if(arg==3){
													strcpy(source,value);
													for(int j=k+1;j<size;j++){
														data[j-k-1]=buf[j];
													}
													//printf(data);
													break;
												}
												memset(value,0,100);
												arg++;
												cnt=0;
											}else{
												value[cnt]=buf[k];
												cnt++;
											}
										}
										if(strcmp(type,"MESSAGE")==0){
											char print[1000];
											strcpy(print,source);
											strcat(print,":");
											strcat(print,data);
											printf(print);
											printf("\n");
										}else{
											printf("%s has invited you to join %s. Accept? (y/n)\n", source, data);
											fgets(fname, 100, stdin);
											if(fname[0]=='y'){
												memset(msgStr,0,1500);
												strcat(msgStr,"JOIN:");
												sprintf(numtostr,"%d", sizeof(data));
												strcat(msgStr,numtostr);
												strcat(msgStr,":");
												strcat(msgStr,clientID);
												strcat(msgStr,":");
												strcat(msgStr,data);
												if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
													perror("send");
												}
												memset(buf,0,MAXBUFLEN);

												if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
													perror("recv");
													exit(1);
												}
												buf[numbytes]='\0';
												if(strcmp(buf,"JN_ACK")==0){
													printf("Session succesfully joined\n");
												}
											}else{
												printf("Rejecting request");
											}

										}
									
									}
								}else if(i==stdin_fd){
									printf("Console to read\n");
									memset(splitStrings,0,sizeof(splitStrings));
									//get command
									fgets(fname, 100, stdin);
									fname[strcspn(fname,"\n")]=0;
									printf(fname);
									printf("\n");
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
									if(strcmp(splitStrings[0],"/leavesession")==0){
										memset(msgStr,0,1500);
										strcat(msgStr,"LEAVE_SESS:");
										sprintf(numtostr,"%d", sizeof(splitStrings[1]));
										strcat(msgStr,numtostr);
										strcat(msgStr,":");
										strcat(msgStr,splitStrings[1]);
										strcat(msgStr,":");
										//strcat(msgStr,splitStrings[1]);
										if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
											perror("send");
										}
										printf("Leaving session...\n");
										leave=true;
										

									}
									if(strcmp(splitStrings[0],"/lobby")==0){

										leave=true;
										

									}
									if(strcmp(splitStrings[0],"/invite")==0){
										memset(msgStr,0,1500);
										strcat(msgStr,"INVITE:");
										sprintf(numtostr,"%d", sizeof(currsess));
										strcat(msgStr,numtostr);
										strcat(msgStr,":");
										strcat(msgStr,splitStrings[1]);
										strcat(msgStr,":");
										strcat(msgStr,currsess);
										if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
											perror("send");
										}

										

									}
									//printf("1");
									if(fname[0]!='/'){
										memset(msgStr,0,1500);
										strcat(msgStr,"MESSAGE:");
										sprintf(numtostr,"%d", sizeof(fname));
										strcat(msgStr,numtostr);
										strcat(msgStr,":");
										strcat(msgStr,currsess);
										strcat(msgStr,":");
										strcat(msgStr,fname);
										if(send(sockfd,msgStr,sizeof(msgStr),0)==-1){
											perror("send");
										}
										// if (select(sockfd+1, &read_fds, NULL, NULL, NULL) == -1) {            
										// 	perror("select");            
										// 	exit(4);  
										// }
										// printf("Found something\n");


										//printf("Looking for server mgs");
										//memset(buf,0,MAXBUFLEN);
										
										// if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
										// 	perror("recv");
										// 	exit(1);
										// }
										// printf("BUF: %s.\n",buf);

									}	
									// if(strcmp(splitStrings[0],"read")==0){
				
									// 	printf("Looking for server mgs");
									// 	memset(buf,0,MAXBUFLEN);
										
									// 	if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
									// 		perror("recv");
									// 		exit(1);
									// 	}
									// 	printf("BUF: %s.\n",buf);

									// }
								}else{
									printf("something else to read\n");
								}


							}


						}
						if(leave){
							break;
						}
						// printf("Looking for server mgs");
						// memset(buf,0,MAXBUFLEN);
						
						// if((numbytes=recv(sockfd,buf,MAX_DATA,0))==-1){
						// 	perror("recv");
						// 	exit(1);
						// }
						// printf("BUF: %s\n",buf);
									  
				

					}

					
					

				}
				//printf("after session joined\n");
				if(strcmp(splitStrings[0],"/logout")==0){
					printf("logging out...\n");
					close(sockfd);
					break;
				}
			}
			//printf("left logged in section");
			continue;

			
		}
		
		if(strcmp(splitStrings[0],"/quit")){break;}
		


	}


	
	
	close(sockfd);

	return 0;
}
