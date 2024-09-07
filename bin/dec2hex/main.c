/* cc -o dec2hex dec2hex.c */

#include <stdio.h>
#include <stdlib.h>

main (argc,argv)
int argc; char *argv[];
{
  unsigned long dec;

  if (argc != 2) {
    printf("\nUsage: %s number\n\n", argv[0]);
    exit(1);
  }

  dec = (unsigned long) atoi(argv[1]);

  printf ("%ld %08x\n", dec, dec);
}
