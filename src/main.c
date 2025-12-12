#include <stdlib.h>
#include <stdio.h>

#include "args.h"
#include "tok/tok.h"


int main(int argc, char* argv[]) {
    printf("\033[1m\033[32m START_COMPILATION \033[0m \n");

    Args args = read_args(argc, argv);

    printf("\033[1m\033[32m END_COMPILATION \033[0m \n");
    return 0;
}