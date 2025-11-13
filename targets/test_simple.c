#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;
    
    char buf[10];
    if (fgets(buf, 10, f) && buf[0] == 'A' && buf[1] == 'B') {
        abort();  // Crash when input is "AB..."
    }
    fclose(f);
    return 0;
}
