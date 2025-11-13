#include <stdio.h>
#include <stdlib.h>

void straight_line_1() {
    int x = 1;
    x = x + 1;
    x = x * 2;
    x = x - 3;
    printf("Result: %d\n", x);
}

void straight_line_2() {
    int y = 10;
    y = y * 3;
    y = y / 2;
    printf("Result: %d\n", y);
}

void straight_line_3() {
    int z = 5;
    z = z + z;
    z = z - 1;
    printf("Result: %d\n", z);
}

void with_branches(int x) {
    if (x > 5) {
        printf("Greater\n");
    } else {
        printf("Less\n");
    }
    
    if (x > 10) {
        printf("Much greater\n");
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    
    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;
    
    int val;
    if (fscanf(f, "%d", &val) == 1) {
        straight_line_1();
        straight_line_2();
        straight_line_3();
        with_branches(val);
        
        if (val == 42) {
            abort();
        }
    }
    
    fclose(f);
    return 0;
}
