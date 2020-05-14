#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
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
#include <fcntl.h>

      
#define PORT 61000 
#define host_addr "127.0.0.1"

#define QUEUE_NAME "/mess_queue_demo"
#define MAX_MESS 10
#define SIZE_MAX_MESS 8192
#define BUF_SIZE_MESS SIZE_MAX_MESS + 10
mqd_t qd_name;
struct mq_attr attr;

int var_condition = 0;
char fname [100];

// for socket
typedef struct
{
    int command_kind;
    unsigned long int leng_payload;
    char payload [255];
    
}mess_header;


mess_header S_sock;


// for message queue
typedef struct
{
    int cmd_queue;
    char queue_buff[255];

}mess_queue;

mess_queue Q_send;


int child_handle_recv(int arg);
int child_handle_send(int arg);

int main(int argc , char *argv[])   
{   
    int child_recv_pid, child_send_pid;
    
    int master_socket , addrlen , new_socket , client_socket[30] ,  
    max_clients = 30 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;        // socket address    

    system("rm -rf /dev/mqueue/*");
    system("sudo clear");
    char buffer[1024];  //data buffer of 1K 

    // Message queue

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESS;
    attr.mq_msgsize = SIZE_MAX_MESS;
    attr.mq_curmsgs = 0;

    if((qd_name = mq_open(QUEUE_NAME, O_RDWR | O_CREAT, 0666, &attr)) == -1)
    {
        perror("mq_open");
        exit(1);
    }         
    //set of socket descriptors  
    fd_set readfds;    
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket     name socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }    
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
    perror("setsockopt(SO_REUSEADDR) failed");
    }

    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 8888   dat ten cho socket
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", PORT);   
    //try to specify maximum of 3 pending connections for the master socket   tao hang doi
    if (listen(master_socket, 10) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
         
    while(1)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
        //wait for an activity on one of the sockets , timeout is NULL, so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n", new_socket, inet_ntoa(address.sin_addr), ntohs (address.sin_port));
            fflush(stdout);
            //     FORK
            
            child_recv_pid = fork();
            if(child_recv_pid < 0)
            {
                perror("fork faile");
            }
            if(child_recv_pid == 0)
            {
                printf("child1: %d \n",getpid());
                fflush(stdout);
                return child_handle_recv(new_socket);
            }


            child_send_pid = fork(); 
            if(child_send_pid == 0)
            {
                printf("child2: %d\n",getpid());
                fflush(stdout);
                return child_handle_send(new_socket);
            }


           // send new connection greeting message  
            if(write(new_socket, "welcome", strlen("Welcome")) != strlen("Welcome"))   
            {   
                perror("send");   
            }   
            printf("Welcome message sent successfully");     
            //   add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("\nClient number: %d\n" , i+1);   
                    fflush(stdout);
                    break;   
                }   
            }   
        }   

        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd, (struct sockaddr*)&address , (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    fflush(stdout);
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    write(sd , buffer , strlen(buffer));   
                }   
            }   
        }   
    }   
    close(master_socket);    
    return 0;   
}   

 // TWO funtion recv and send

int child_handle_recv(int arg)
{
    int new_socket = arg;
    char recv_buff_socket [8192];
    mq_send(qd_name, "Gui mess thong qua message QUEUE", strlen("Gui mess thong qua message QUEUE"), 0);

    while(1)
    {
        int size_recv;
        memset(recv_buff_socket, 0x00, sizeof(recv_buff_socket));
     
       
        
        size_recv = read(new_socket, recv_buff_socket, sizeof(mess_header));
        if(size_recv == 0)
        {
            puts("client disconnect - close socket");
            return;
        }
        if(size_recv < 0)
        {
            puts("recv fail");
        }
        if( size_recv != sizeof(mess_header))
        {
            puts("Lack of Byte");
        }
        
        
        mess_header *R_sock = (mess_header *)&recv_buff_socket;
        printf("Kind = %d\n",R_sock->command_kind);
        // get list
        if(R_sock->command_kind == 1)
        {
            puts("\nClient request list of files");
            Q_send.cmd_queue = 1;
           
            if(mq_send(qd_name, &Q_send, sizeof(mess_queue), 0) == -1)
            {
                perror("mq_send recv child");
                fflush(stdout);
            }
            else
            {
                puts("R-l ->\n");
                fflush(stdout);
            }
        }

        // get file
        else if(R_sock->command_kind == 2)
        {
            Q_send.cmd_queue = 2;
            memcpy(Q_send.queue_buff, R_sock->payload, sizeof(R_sock->payload));
            puts("\nClient request File name:\t");
            // printf("%s\n", R_sock.payload);
            printf("%s\n", Q_send.queue_buff);
            fflush(stdout);
            
            if(mq_send(qd_name, &Q_send, sizeof(mess_queue), 0) == -1)
            {
                perror("mq_send comand 2");
            }
            else
            {
                puts("R-f ->\n");
                fflush(stdout);
            }
        }
    }
}



int child_handle_send(int arg)
{
    int new_socket = arg;
    char in_buf[8192];
    char list_buff [255] = "";
    memset(in_buf, 0x00, sizeof(in_buf));
    
    ssize_t recv_q;
    recv_q = mq_receive(qd_name, in_buf, SIZE_MAX_MESS, 0);
    printf("%s\n", in_buf);
    fflush(stdout);
    

    while(1)
    {
        memset(in_buf, 0x00, sizeof(in_buf));
        recv_q = mq_receive(qd_name, in_buf, SIZE_MAX_MESS, 0);
        mess_queue *Q_recv = (mess_queue *)&in_buf;
        if(recv_q == -1)
        {
            perror("mq receive child: \n");
            fflush(stdout);
        }
        else 
        {
            
            if( Q_recv->cmd_queue == 1 )  // get list
            {
                S_sock.command_kind = 81;
                memset (list_buff, 0, sizeof(list_buff));
                puts("-> S-l");
                fflush(stdout);
                // get list
                DIR *d;
                struct dirent *dir;
                d = opendir(".");
                if(d)
                {
                    while ((dir = readdir(d)) !=NULL)
                    {
                        // printf("%s\n", dir->d_name);
                        strcat (list_buff, dir->d_name); 
                        strcat (list_buff, "\n");   
                    }
                    closedir(d);
                }
                memcpy(S_sock.payload, list_buff, sizeof(list_buff));
                if(write(new_socket, &S_sock, sizeof(mess_header)) == -1)   
                {
                    perror("write list file");
                }
                printf("%ld\n", strlen(list_buff));
                printf("%s\n", S_sock.payload);
                fflush(stdout); 
            }
              
//////////////////////////////////////////////////////////////////////////////////////////////
            if (Q_recv->cmd_queue == 2)  // get file
            {
                S_sock.command_kind = 82;
                strcpy(S_sock.payload, Q_recv->queue_buff);
                puts("-> S-f");

                strcpy(fname, Q_recv->queue_buff);
                printf("%s\n", Q_recv->queue_buff);
                fflush(stdout);

                //write(new_socket, fname, 256);
                unsigned long fsize = 0;

                FILE *fs = fopen(fname, "r");
                if( fs == NULL)
                {
                    printf("file open error");
                }
                fseek(fs, 0L, SEEK_END);
                fsize = ftell(fs);
                memcpy(&S_sock.leng_payload, &fsize, sizeof(fsize));
                if( write(new_socket, &S_sock, sizeof(mess_header))  < 0 )
                {
                    perror( "write file name" );
                }
                printf("Size: %lu bytes\n", fsize);
                fflush(stdout);
                fclose(fs);
                
                FILE *fp;
                fp = fopen(fname, "rb");
                if(fp == NULL)
                {
                    perror("fopen file");
                    return 1;
                }
                while(1)
                {
                    int size_send=0;
                    unsigned char buff_send_file[1024] = {0};
                    int nread = fread(buff_send_file, 1, 1024, fp);
                    if(nread > 0)
                    {
                        size_send = write(new_socket, buff_send_file, nread);
                        if(size_send != nread)
                        {
                            puts("send file fail");
                            break;
                        }

                    }
                    if(nread < 1024)
                    {       
                        if(feof(fp))
                        {
                            printf("End of file\n");
                            printf("Transfer Done\n");
                        }
                        if(ferror(fp))
                        {
                            printf("Error reading\n");
                        }
                        break;
                    }
                }
                close(new_socket);
                shutdown(new_socket,SHUT_WR);
            } 
        }
    }
}
