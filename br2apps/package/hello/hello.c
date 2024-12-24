#include <stdio.h>

void drawChristmasTree(int height) {
    int i, j, k;

    // Fő háromszög része a karácsonyfának
    for (i = 1; i <= height; i++) {
        // Szóközök
        for (j = i; j < height; j++) {
            printf(" ");
        }
        // Csillagok
        for (k = 1; k <= (2 * i - 1); k++) {
            printf("*");
        }
        printf("\n");
    }

    // Törzs
    int trunkWidth = height / 3;
    if (trunkWidth % 2 == 0) {
        trunkWidth++; // Legyen páratlan a szárszélesség
    }
    int trunkHeight = height / 4;
    int spacesBeforeTrunk = height - (trunkWidth / 2) - 1;

    for (i = 0; i < trunkHeight; i++) {
        for (j = 0; j < spacesBeforeTrunk; j++) {
            printf(" ");
        }
        for (k = 0; k < trunkWidth; k++) {
            printf("*");
        }
        printf("\n");
    }
}

int main() {
    int height;

    printf("Add meg a karácsonyfa magasságát: ");
    scanf("%d", &height);

    if (height < 3) {
        printf("A karácsonyfa magasságának legalább 3-nak kell lennie!\n");
        return 1;
    }

    drawChristmasTree(height);

    return 0;
}

