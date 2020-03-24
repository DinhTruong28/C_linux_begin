#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


unsigned long odd = 0;

pthread_mutex_t lock_countodd;

void *count_odd(void *arg)
{
	unsigned long n = *(unsigned long*)arg;
	pthread_mutex_lock (&lock_countodd);
	for (unsigned long i = 0; i < n; i++) {
		if (i%2 != 0)
			odd ++;
	}
	pthread_mutex_unlock(&lock_countodd);
	return NULL;
}

void main()
{
	clock_t start_time;
	clock_t start_time2;
	clock_t	finish_time;
	clock_t	finish_time2;
	
	pthread_mutex_init (&lock_countodd, NULL);
	unsigned long count = 2000000000;
	unsigned long count0 = 500000000;
	unsigned long count1 = 500000000;
	unsigned long count2 = 500000000;
	unsigned long count3 = 500000000;
	
	
	pthread_t thread0;
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	

	start_time = clock();
	odd = 0;
	count_odd((void *) &count);
	finish_time = clock();
	printf("Use 1 thread, %f sec to counting odd = %lu\n", (double)(finish_time - start_time)/CLOCKS_PER_SEC, odd);

	start_time2 = clock();
	odd = 0;
	pthread_create(&thread0, NULL, count_odd, (void*) &count0);
	pthread_create(&thread1, NULL, count_odd, (void*) &count1);
	pthread_create(&thread2, NULL, count_odd, (void*) &count2);
	pthread_create(&thread3, NULL, count_odd, (void*) &count3);
	
	
	pthread_join(thread0, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	
	
	pthread_mutex_destroy(&lock_countodd);

	finish_time2 = clock();
	printf("Use 4 threads, %f sec to counting odd = %lu\n",(double)(finish_time2 - start_time2)/CLOCKS_PER_SEC, odd);
}