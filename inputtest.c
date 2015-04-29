#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    for(int i = 0; i < 5; i++) {
        printf("%s ", argv[argc - 1]);
    }
    printf("\n");
    exit(0);
}