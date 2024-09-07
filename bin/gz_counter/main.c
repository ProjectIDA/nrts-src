/* cc -o gz_counter gz_counter.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (argc,argv)
int argc; char *argv[];
{

  char teename[] = "SSSSSSSSCCCCCCCCCCCCCCCC"; /* 32-bit sig, 64-bit counter */
  long long counter;

  if (argc != 2) {
    printf("\nUsage: %s fname\n\n", argv[0]);
    exit(1);
  }

  if (strlen(argv[1]) == strlen(teename)) {
        strcpy(teename, argv[1]);
        counter = strtoll(&teename[8], NULL, 16);
    }
  printf ("%s %lld\n", teename,counter);
}
