#include <stdio.h>
#include <string.h>   //strlen  
#include <stdlib.h>    
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <mqueue.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>


#define PORT 61000 
#define host_addr "127.0.0.1"


// for socket
typedef struct
{
    int command_kind;
    unsigned long int leng_payload;
    char payload [255];
    
}mess_header;       

	// gotoxy()
void gotoxy(int x, int y)
{
    printf("%c[%d;%df", 0x1B, y, x);
}

int main()
{
	system("clear");
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char recv_buff[32000] = "";
	memset(recv_buff, 0, sizeof(recv_buff));

	char c[100] = "";


	mess_header S_sock;
	mess_header *R_sock;
	
	size_t recv_s;
	unsigned long fsize = 0;
    char fname [100];
    char fsz[20];
	int bytes_recv = 0;

    

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}

	printf("[+]Client Socket is created.\n");
	memset(&serverAddr, 0x00, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(host_addr);

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}

	printf("[+]Connected to Server.\n");
	printf("\nSERVER: \t");
	read(clientSocket, recv_buff, sizeof(recv_buff));
	printf("%s\n", recv_buff);
	fflush(stdout);
	puts("\nprepare message to send: \n");
	
	while(1)
	{
		printf("-CLIENT: \t");
		memset(c, 0, sizeof(c));
		char c[100] = "";
		char f[255] = "";
		fflush(stdin);
		scanf("%s", (char *)&c);
		memset(&S_sock,0xFF,sizeof(mess_header));
		if(strcmp(c, "1") == 0)
		{
			puts("getlist");
			int iRet;
			S_sock.command_kind = 1;
			iRet = write(clientSocket, &S_sock, sizeof(mess_header));
			if( iRet == -1)
			{
				perror("send list");
			}
			else if( iRet != sizeof(mess_header))
			{
				puts("Lack of byte");
			}
			
			printf("Kind: %d\n SendByte: %d \n",S_sock.command_kind,iRet);
			puts("SEND REQUEST TO GET LISTS FILE\n");
			fflush(stdout);  
		}

		if(strcmp(c, "2") == 0)
		{
			puts("getfile");
			S_sock.command_kind = 2;
			int iRet;
			printf("typing file name:\t");
			scanf("%s", (char *)&f);
			memcpy(S_sock.payload, &f, sizeof(f));
			iRet = write(clientSocket, &S_sock, sizeof(mess_header));
			{
				perror("send get file");
			}
			if( iRet == -1)
			{
				perror("send list");
			}
			else if( iRet != sizeof(mess_header))
			{
				puts("Lack of byte");
			}
			printf("Kind: %d\n",S_sock.command_kind);
			printf("\nGETTING FILE NAME: %s\n", S_sock.payload);
		}

		if(strcmp(c, ":exit") == 0)
		{
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}


		memset(recv_buff, 0, sizeof(recv_buff));
        recv_s = read(clientSocket, recv_buff, sizeof(recv_buff));
        mess_header *R_sock = (mess_header *)&recv_buff;
		//recv_s = read(clientSocket, recv_buff, strlen(recv_buff));
		if( recv_s == -1)
		{
			perror("receive list\n");
		}
        else
        {
            while (recv_s > 0)
            {
                if(R_sock->command_kind == 81)
                {
					printf("-SERVER:\n");
                    puts("[+] List:\n");
                    	
                    printf("%s\n", R_sock->payload);
                    fflush(stdout);
                    break;
                }


                else if(R_sock->command_kind == 82)
                {
                    puts("[+] File\n");
					fflush(stdout);
                    strcpy(fname, R_sock->payload);
					puts(fname);
                    FILE *fp;
                    printf("Receiving file ...");
                    printf("File name: %s\n", fname);
                    fp = fopen(fname, "ab");
                    if(fp == NULL)
                    {
                        puts("error open file");
                        return 1;
                    }
                    long double sz = 1;
                    memset(recv_buff, 0, sizeof(recv_buff));
                    while ((bytes_recv = read(clientSocket, recv_buff, 1024 )) > 0)
                    {
                        sz++;
                        gotoxy(0, 5);
                        //printf("Received:   %0.02f Mb", (sz/1024));
                        printf("Received: \033[0;32m %llf Mb\033[0m", (sz/1024));
                        fflush(stdout);
                        gotoxy(0, 6);
                        unsigned long int per = (unsigned long) sz*1024;
                        float pers = ((float) per / (float) fsize)*100;
                        printf("Completed: [%0.2f%%]    \n", pers); 
                        fflush(stdout);
                        //printProgress(pers); 
                        // recvBuff[n] = 0;
                        fwrite(recv_buff, 1,bytes_recv, fp);
                        // printf("%s \n", recvBuff);
                    }

                    if(bytes_recv < 0)
                    {
                        printf("\n Read Error \n");
                    }
                        printf("\nFile OK....Completed\n");	
                        break; 
                }    
            }
        }
	}
	return 0;
}
