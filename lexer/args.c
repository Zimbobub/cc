#include "args.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char* optarg;
extern int optopt;

Args read_args(int argc, char* argv[]) {
    Args args = {0};

    int opt;
    
    // put ':' in the starting of the
    // string so that program can 
    //distinguish between '?' and ':' 
    while((opt = getopt(argc, argv, ":i:o:")) != -1) { 
        switch(opt) {
            case 'i': 
                printf("input: %s\n", optarg);
                args.input_file = optarg;
                break; 
            case 'o': 
                printf("output: %s\n", optarg); 
                args.output_file = optarg;
                break; 
            case ':': 
                printf("option needs a value\n");
                exit(EXIT_FAILURE);
                break; 
            case '?': 
                printf("unknown option: %c\n", optopt);
                exit(EXIT_FAILURE);
                break; 
        } 
    } 
    
    if (args.input_file == NULL) {
        printf("no -i input file supplied\n");
        exit(EXIT_FAILURE);
    }
    if (args.output_file == NULL) {
        printf("no -o output file supplied\n");
        exit(EXIT_FAILURE);
    }

    return args;
}