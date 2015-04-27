#include <stdio.h>
#include <stdlib.h>

int main() {
	printf("\n\nbackground process, pid = %d\n\n\n", (int)getpid());
	exit(0);
}