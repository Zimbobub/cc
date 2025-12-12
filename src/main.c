#include <stdlib.h>
#include <stdio.h>



int main(int argc, char* argv[]) {
    printf("START_COMPILATION\n");
    for (size_t i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    printf("END_COMPILATION\n");
    return 0;
}