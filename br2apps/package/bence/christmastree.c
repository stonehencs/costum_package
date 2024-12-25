#include <stdio.h>

void print_tree(int height) {
    for (int i = 1; i <= height; i++) {
        for (int j = 0; j < height - i; j++) {
            printf(" ");
        }
        for (int j = 0; j < (2 * i - 1); j++) {
            printf("*");
        }
        printf("\n");
    }
    for (int i = 0; i < height - 1; i++) {
        printf(" ");
    }
    printf("|\n");
}

int main() {
    int height;
    printf("Enter the height of the Christmas tree: ");
    scanf("%d", &height);
    print_tree(height);
    return 0;
}

