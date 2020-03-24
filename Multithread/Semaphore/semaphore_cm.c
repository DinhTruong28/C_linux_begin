#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#define MAX_NUM_FILE 20

sem_t sem_write;
pthread_mutex_t lock_valid_file;
char files_info[MAX_NUM_FILE] ;

void write_to_file( int n)
{
	int i = 0;
	int ok = 0;
	while(!ok)
	for (i = 0 ; i < n ; i ++)
	{
		pthread_mutex_lock(&lock_valid_file);
		if(files_info[i] == 0)
		{
			files_info[i] = 1;
			ok = 1;
		}
		pthread_mutex_unlock(&lock_valid_file);
		// ghi vao file neu ok o tren 
		if( ok == 1)
		{
			time_t t;
			char nameFile[30];
			sprintf(nameFile ,"fsemaphore%d.txt" , i);
			FILE *fp  = fopen(nameFile , "a+");                    // thuoc tinh write
           time(&t);
			fprintf( fp , "\n %lu ok write to file %d at %s \n\n", pthread_self(), i, ctime(&t));
			sleep(2);
			files_info[i] = 0;
			fclose(fp);
			break;
		}
	}
}
void* thread(void* arg) 
{
	//wait 
	sem_wait(&sem_write);      // block neu het khoa

	write_to_file(5);   

	sem_post(&sem_write);     //tra lai khoa
}

int main() 
{
	pthread_t t[10];
	int i = 0, nthread = 9;
	pthread_mutex_init(&lock_valid_file, NULL );
	sem_init(&sem_write, 0, 10);          // gia tri 0 cho phép chia se giữa cac thread trong process,
                                        // gia trị khac 0 cho phep chia se giua cac process khac
	
	pthread_create(&t[0],NULL,thread , NULL); 
    pthread_create(&t[1],NULL,thread , NULL); 
    pthread_create(&t[2],NULL,thread , NULL); 
    pthread_create(&t[3],NULL,thread , NULL); 
    pthread_create(&t[4],NULL,thread , NULL); 
    pthread_create(&t[5],NULL,thread , NULL); 
    pthread_create(&t[6],NULL,thread , NULL); 
    pthread_create(&t[7],NULL,thread , NULL); 
    pthread_create(&t[8],NULL,thread , NULL); 
    pthread_create(&t[9],NULL,thread , NULL); 


	

	for ( i = 0 ; i < nthread ; i ++)
	{
	pthread_join(t[i],NULL); 
	}
	
	pthread_mutex_destroy(&lock_valid_file);
	sem_destroy(&sem_write); 
	printf("ok");
return 0;
}