#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char s[128];
    scanf("%[^\n]", s);
    printf("%s\n", s);
    return 0;
}
