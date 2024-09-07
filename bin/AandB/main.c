/* cc -o AandB AandB.c */
/* AandB compares two numbers.  It returns 2, if the n1 is smaller than n2. */
/* AandB is used in r2q_dataseg */

#include <stdlib.h>
#include <stdio.h>

main (argc,argv)
int argc; char *argv[];
{

  double a, b;

  if (argc != 3) {
    printf("\nUsage: %s number1 number2 \n\n", argv[0]);
    exit(1);
  }

  a = (double) atof (argv[1]);
  b = (double) atof (argv[2]);
/*  printf ("%f %f\n", a, b); */
  if (a < b) return (2);
  return (1);
}
