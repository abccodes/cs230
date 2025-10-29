#include <stdio.h>
#include <string.h>
int main(int argc, char** argv) {
    if (argc < 2) return 0;
    FILE* f = fopen(argv[1], "rb");
    if (!f) return 0;
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    if (n > 3 && buf[0] == 'B' && buf[1] == 'A' && buf[2] == 'D') {
        *((volatile int*)0) = 1; // intentional crash
    }
    if (memmem(buf, n, "AAAA", 4)) puts("hit");
    return 0;
}
