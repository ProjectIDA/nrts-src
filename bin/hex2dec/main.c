/* cc -o hex2dec hex2dec.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (argc,argv)
int argc; char *argv[];
{

  char teename[] = "CCCCCCCCCCCCCCCC"; /* 64-bit counter */
  long long counter;

  if (argc != 2) {
    printf("\nUsage: %s fname\n\n", argv[0]);
    exit(1);
  }

  strcpy(teename, argv[1]);
  counter = strtoll(teename, NULL, 16);
  printf ("%s %lld\n", teename,counter);
}
