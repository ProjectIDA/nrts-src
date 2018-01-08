#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

	// printf("CMD: %s\n", argv[1]);

	FILE *pout;
	int MAXLINE = 256;
	char line[MAXLINE];
	char *cmd;

	if ((cmd = malloc(strlen(argv[1]))) == NULL) {
		printf("Error allocating memory\n");
		return -1;
	}

	strcpy(cmd, argv[1]);

	pout = popen(cmd, "r");

	while (fgets(line, MAXLINE, pout) != NULL) {
		printf("%s", line);
	}

	int res = pclose(pout);
	printf("PING Result: %d\n", res);
	
	return 0;
}

