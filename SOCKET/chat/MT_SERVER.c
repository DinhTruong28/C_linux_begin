
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
	time_t ticks;
	 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
     
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(9898);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
	 
	
    listen(sockfd, 5);      // 5 BACKLOG
	printf("Waiting connection ...\n");
    clilen = sizeof(cli_addr);
    if((newsockfd = accept(sockfd,  (struct sockaddr *) &cli_addr,  &clilen))==0)
	

	time(&ticks);
	sprintf(buffer, "Server reply %s", ctime(&ticks));
	write(newsockfd, buffer, strlen(buffer));

    if (newsockfd < 0) 
          error("ERROR on accept");
    while(1)
     {	
		printf("Client join\n");
    	bzero(buffer, 256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("Client: %s\n",buffer);
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(newsockfd,buffer,strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
        int i=strncmp("Bye" , buffer, 3);
        if(i == 0)
            break;
     }
    close(newsockfd);
    close(sockfd);
    return 0; 
}