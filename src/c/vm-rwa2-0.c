#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Missing argument.\n");
        exit(1);
    }
    FILE* f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);

    rewind(f);

    unsigned char* mem = malloc(size);
    size_t got = fread(mem, 1, size, f);
    assert(got == size);
    (void)got;
    fclose(f);
    int pc = 0;

    while (1) {
        char inst = mem[pc++];
        switch (inst) {
            case 0: // Halt
                exit(0);

            default:
                printf("unknown instruction %d\n", inst);
                exit(1);
        }
    }

    return 0;    
}