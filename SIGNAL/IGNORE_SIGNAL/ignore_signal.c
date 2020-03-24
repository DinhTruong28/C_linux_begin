#include  <stdio.h>
#include  <signal.h>
#include <unistd.h>

#define   MAX_i    10
#define   MAX_j    20000

void  main(void)
{
     int            i;
     unsigned long  j, sum;

     signal(SIGINT, SIG_IGN);        
     printf("bat dau vong lap, signal bi IGNORE, Ctrl + C KHONG co tac dung .\n");
     for (i = 1; i <= MAX_i; i++)  {
          sum = 0;
          for (j = 0; j <= MAX_j; j++)
               sum += j;
               printf("sum = %ld\n", sum);
               sleep(1);
     }
     printf("Done\n\n");

     signal(SIGINT, SIG_DFL);     
     printf("khoi dong lai vong loop, Ctrl + C co tac dung ...\n");
     for (i = 1; i <= MAX_i; i++)  {
          sum = 0;
          for (j = 0; j <= MAX_j; j++)
               sum += j;
               printf("sum = %ld\n", sum);
               sleep(1);
     }
     printf("Done\n");
}
