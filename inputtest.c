#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    printf("%s", argv[1]);
    fflush(stdin);
    exit(0);
}