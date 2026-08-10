#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char* input_file;
    char* output_file;
} Args;


Args read_args(int argc, char* argv[]);