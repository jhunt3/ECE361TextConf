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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define MAXBUFLEN 1500
#define PORT "8000"

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {        
        return &(((struct sockaddr_in*)sa)->sin_addr);    
        }    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
struct clientInfo{
    char clientID[50];
    char password[50];
    char sessionID[20][50];
    char clientIP[50];
    int clientPort;
};



int main(void){

    struct clientInfo clientData[5];
    strcpy(clientData[0].clientID,"client0");
    strcpy(clientData[0].password,"password0");
    strcpy(clientData[1].clientID,"client1");
    strcpy(clientData[1].password,"password1");
    strcpy(clientData[2].clientID,"client2");
    strcpy(clientData[2].password,"password2");
    strcpy(clientData[3].clientID,"client3");
    strcpy(clientData[3].password,"password3");
    strcpy(clientData[4].clientID,"client4");
    strcpy(clientData[4].password,"password4");





    fd_set master;    // master file descriptor list    
    fd_set read_fds;  // temp file descriptor list for select()    
    int fdmax;        // maximum file descriptor number    
    int listener;     // listening socket descriptor    
    int newfd;        // newly accept()ed socket descriptor    
    struct sockaddr_storage remoteaddr; // client address    
    socklen_t addrlen;    
    char buf[1500];    // buffer for client data    
    int nbytes;    
    char remoteIP[INET_ADDRSTRLEN];    
    int yes=1;        // for setsockopt() SO_REUSEADDR, below    
    int i, j, rv;    
    struct addrinfo hints, *ai, *p;    
    FD_ZERO(&master);    // clear the master and temp sets    
    FD_ZERO(&read_fds);    // get us a socket and bind it    
    memset(&hints, 0, sizeof hints);    
    hints.ai_family = AF_UNSPEC;    
    hints.ai_socktype = SOCK_STREAM;    
    hints.ai_flags = AI_PASSIVE;    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {        
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));        
        exit(1);    
    }    
    for(p = ai; p != NULL; p = p->ai_next) {        
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);        
        if (listener < 0) {             
            continue;        
        }        // lose the pesky "address already in use" error message        
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));        
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);            
            continue;        
        }        
        break;    
    }
    // if we got here, it means we didn't get bound    
    if (p == NULL) {        
        fprintf(stderr, "selectserver: failed to bind\n");        
        exit(2);    
    }
    freeaddrinfo(ai); // all done with this    
    // listen    
    if (listen(listener, 10) == -1) {        
        perror("listen");        
        exit(3);    
    }
    // add the listener to the master set    
    FD_SET(listener, &master);    
    // keep track of the biggest file descriptor    
    fdmax = listener; // so far, it's this one

    // main loop    
    for(;;) {        
        read_fds = master; // copy it        
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {            
            perror("select");            
            exit(4);        
        }        
        
        // run through the existing connections looking for data to read        
        for(i = 0; i <= fdmax; i++) {            
            if (FD_ISSET(i, &read_fds)) { // we got one!!                
                if (i == listener) {                    
                    // handle new connections                    
                    addrlen = sizeof remoteaddr;                    
                    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);                    
                    if (newfd == -1) {                        
                        perror("accept");                    
                    } else {                        
                        FD_SET(newfd, &master); // add to master set                        
                        if (newfd > fdmax) {    // keep track of the max                            
                        fdmax = newfd;                        }                        
                        printf("selectserver: new connection from %s on socket %d\n",
                        inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP, INET_ADDRSTRLEN),newfd);

                    }                
                } else {                    
                    // handle data from a client
                    memset(buf,0,MAXBUFLEN);                   
                    if ((nbytes = recv(i, buf, MAXBUFLEN, 0)) <= 0) {
                        // got error or connection closed by client                        
                        if (nbytes == 0) {                            
                            // connection closed                            
                            printf("selectserver: socket %d hung up\n", i);                        
                        } else {                            
                            perror("recv");                        
                        }                        
                        close(i); // bye!                        
                        FD_CLR(i, &master); // remove from master set                    
                    } else {   

                        printf("selectserver: socket %d sent message\n", i);
                        printf(buf);
                        printf("\n");
                        

                        char type[20];
                        char value[100];
                        int size;
                        char source[50];
                        memset(source,0,50);
                        char data[1000];
                        int cnt=0;
                        int arg=1;
                        printf(buf);
                        printf("\n");
                        memset(value,0,100);
                        for(int k=0;k<500;k++){
                            //printf("%d",k);
                            if(buf[k]==':'){

                                if(arg==1){
                                    strcpy(type,value);
                                    memset(value,0,100);
                                }else if(arg==2){
                                    size=atoi(value);
                                    memset(value,0,100);
                                }else if(arg==3){
                                    strcpy(source,value);
                                    for(int j=k+1;j<size;j++){
                                        data[j-k-1]=buf[j];
                                    }
                                    memset(value,0,100);
                                    break;
                                }
				                
                                arg++;
                                cnt=0;
                            }else{
                                value[cnt]=buf[k];
                                cnt++;
                            }
                        }
                        printf("Found command:");
                        printf(type);
                        printf("\n");
                        printf(source);
                        printf("\n");
                        printf(data);
                        printf("\n");
                        char sess[50];
                        memset(sess,0,50);
                        strcpy(sess,data);
                        bool loginsuccess=false;

                        if(strcmp(type,"LOGIN")==0){
                            printf("Received login request\n");
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){
                                printf(source);
                                printf(clientData[k].clientID);
                                printf("\n");
                                if(strcmp(source,clientData[k].clientID)==0){
                                    
                                    if(strcmp(data,clientData[k].password)==0){
                                        if (send(i, "LO_ACK", sizeof("LO_ACK"), 0) == -1) {                                        
                                            perror("send");                                    
                                        }
                                        clientData[k].clientPort=i;
                                        loginsuccess=true;
                                        break;
                                    }
                                }

                            }
                            if(!loginsuccess){
                                if (send(i, "LO_NAK", sizeof("LO_NAK"), 0) == -1) {                                        
                                    perror("send");                                    
                                }
                            }
                        }
                        

                        if(strcmp(type,"NEW_SESS")==0){
                            printf("Received new session request\n");
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){

                                if(strcmp(source,clientData[k].clientID)==0){
                                    printf("found clientdata\n");
                                    strcpy(clientData[k].sessionID[0],data);
                                    if (send(i, "NS_ACK", sizeof("NS_ACK"), 0) == -1) {                                        
                                        perror("send");                                    
                                    }
                                    break;
                                }


                            }

                        }

                        if(strcmp(type,"JOIN")==0){
                            printf("Received join request\n");
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){

                                if(strcmp(source,clientData[k].clientID)==0){
                                    printf("found clientdata\n");
                                    bool exists=false;
                                    for(int counter=0;counter<20;counter++){
                                        //printf("Test: %s.\n",clientData[k].sessionID[counter]);
                                        if(strcmp(clientData[k].sessionID[counter],data)==0){
                                            exists=true;
                                            break;
                                            
                                        }

                                    }
                                    if(exists){
                                        break;
                                    }
                                    for(int counter=0;counter<20;counter++){
                                        printf("Test: %s.\n",clientData[k].sessionID[counter]);
                                        if(strcmp(clientData[k].sessionID[counter],"")==0){
                                            strcpy(clientData[k].sessionID[counter],data);
                                            break;
                                        }

                                    }
                                    for(int counter=0;counter<20;counter++){

                                        printf("%s\n",clientData[k].sessionID[counter]);

                                    }
                                    //strcpy(clientData[k].sessionID,data);

                                    break;
                                }


                            }
                            if (send(i, "JN_ACK", sizeof("JN_ACK"), 0) == -1) {                                        
                                perror("send");                                    
                            }

                        }
                        if(strcmp(type,"LEAVE_SESS")==0){
                            printf("Received leave session request\n");
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){

                                if(strcmp(source,clientData[k].clientID)==0){
                                    printf("found clientdata\n");
                                    for(int counter=0;counter<20;counter++){
                                        if(strcmp(clientData[k].sessionID[counter],data)==0){
                                            memset(clientData[k].sessionID[counter],0,50);
                                        }
                                    }
                                  

                                    break;
                                }


                            }

                        }
                        if(strcmp(type,"INVITE")==0){
                            printf("Received invite request\n");
                            // if (send(i, "MSG_CFM", sizeof("MSG_CFM"), 0) == -1) {                                        
                            //     perror("send");                                    
                            // }
                            printf("%s\n",buf);
                            char inviter[50];
                            memset(inviter,0,50);
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){
                                if(clientData[k].clientPort==i){
                                    strcpy(inviter,clientData[k].clientID);
                                    break;

                                }

                            }
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){
                                if(strcmp(source,clientData[k].clientID)==0){
                                    char msgStr[1500];
                                    char numtostr[100];
                                    memset(msgStr,0,1500);
                                    memset(numtostr,0,1500);
                                    strcat(msgStr,"INVITE:");
                                    sprintf(numtostr,"%d", sizeof(data));
                                    strcat(msgStr,numtostr);
                                    strcat(msgStr,":");
                                    strcat(msgStr,inviter);
                                    strcat(msgStr,":");
                                    printf("DATA: %s\n",sess);
                                    printf(data);
                                    strcat(msgStr,sess);
                                    printf("%s\n",msgStr);
                                    if (send(clientData[k].clientPort, msgStr, sizeof(msgStr), 0) == -1) {                                        
                                        perror("send");                                    
                                    }

                                }
                                printf("Source: %s\n",source);

                                //for(int counter=0;counter<20;counter++){
                                //     if(strcmp(source,clientData[k].sessionID[counter])==0 && clientData[k].clientPort!=i){
                                //         printf("invite message to: ");
                                //         printf(clientData[k].clientID);
                                //         printf("\n");
                                //         if (send(clientData[k].clientPort, buf, sizeof(buf), 0) == -1) {                                        
                                //             perror("send");                                    
                                //         }
                                //         break;
                                //     }

                                // }


                            }
                            

                        }
                        if(strcmp(type,"MESSAGE")==0){
                            printf("Received message request\n");
                            // if (send(i, "MSG_CFM", sizeof("MSG_CFM"), 0) == -1) {                                        
                            //     perror("send");                                    
                            // }
                            printf("%s\n",buf);
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){
                                printf("Source: %s\n",source);
                                for(int counter=0;counter<20;counter++){
                                    if(strcmp(source,clientData[k].sessionID[counter])==0 && clientData[k].clientPort!=i){
                                        printf("forwarding message to: ");
                                        printf(clientData[k].clientID);
                                        printf("\n");
                                        if (send(clientData[k].clientPort, buf, sizeof(buf), 0) == -1) {                                        
                                            perror("send");                                    
                                        }
                                        break;
                                    }

                                }

                                // if(strcmp(source,clientData[k].clientID)==0){

                                //     printf("found clientdata\n");
                                //     //memset(clientData[k].sessionID,0,1500);
                                //     for(int m=0;m<(sizeof(clientData)/sizeof(struct clientInfo));m++){
                                //         if(strcmp(clientData[k].sessionID,clientData[m].sessionID)==0 && k!=m){
                                //             printf("forwarding message to: ");
                                //             printf(clientData[m].clientID);
                                //             printf("\n");

                                //             if (send(clientData[m].clientPort, buf, sizeof(buf), 0) == -1) {                                        
                                //                 perror("send");                                    
                                //             }
                                //         }
                                //     }

                                //     break;
                                // }


                            }
                            

                        }
                        if(strcmp(type,"QUERY")==0){
                            char msgStr[1500];
                            memset(msgStr,0,1500);

                            //printf("Received join request\n");
                            for(int k=0;k<(sizeof(clientData)/sizeof(struct clientInfo));k++){
                                strcat(msgStr,clientData[k].clientID);
                                strcat(msgStr,":");
                                for(int counter=0;counter<20;counter++){
                                    strcat(msgStr,clientData[k].sessionID[counter]);
                                    strcat(msgStr," ");
                                }
                                
                                strcat(msgStr,";");

                            }
                            if (send(i, msgStr, sizeof(msgStr), 0) == -1) {                                        
                                perror("send");                                    
                            }

                        }
                        // we got some data from a client                        
                        // for(j = 0; j <= fdmax; j++) {                            
                        //     // send to everyone!                            
                        //     if (FD_ISSET(j, &master)) {                                
                        //         // except the listener and ourselves                                
                        //         if (j != listener && j != i) {                                    
                        //             if (send(j, buf, nbytes, 0) == -1) {                                        
                        //                 perror("send");                                    
                        //             }                                
                        //         }                            
                        //     }                        
                        // }     
              
                    }                
                } // END handle data from client            
            } // END got new incoming connection        
        } // END looping through file descriptors    
    } // END for(;;)--and you thought it would never end!    
    return 0;
}
