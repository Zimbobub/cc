#pragma once


typedef struct {
    char* input_file;
    char* output_file;
} Args;


Args read_args(int argc, char* argv[]);