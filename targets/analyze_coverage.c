#include <stdio.h>
#include <stdlib.h>

void function_a() {
    printf("Function A\n");
}

void function_b() {
    printf("Function B\n");
}

void function_c() {
    printf("Function C\n");
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    
    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;
    
    char buf[10];
    if (fgets(buf, 10, f)) {
        if (buf[0] == 'A') {
            function_a();
            
            if (buf[1] == 'B') {
                function_b();
                
                if (buf[2] == 'C') {
                    function_c();
                    abort();
                }
            }
        }
    }
    
    fclose(f);
    return 0;
}
