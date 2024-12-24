#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <string.h>

void shift_out(struct gpiod_line *data_line, struct gpiod_line *clock_line, struct gpiod_line *latch_line, const char *data) {
    // Latch reset (zárva tartás adatírás közben)
    gpiod_line_set_value(latch_line, 0);
    usleep(1000); // 1ms késleltetés

    // Bitenkénti adatkiírás
    for (int i = 0; i < 8; i++) {
        int bit = data[i] - '0'; // Bináris érték (0 vagy 1)
        gpiod_line_set_value(data_line, bit); // Adat kimenet
        usleep(1000); // Rövid késleltetés
        gpiod_line_set_value(clock_line, 1); // Órajel impulzus
        usleep(1000);
        gpiod_line_set_value(clock_line, 0);
    }

    // Latch beállítása (adat érvényesítése)
    gpiod_line_set_value(latch_line, 1);
    usleep(1000);
    gpiod_line_set_value(latch_line, 0);
}

int main() {
    char chip_name[32];
    int data_pin, clock_pin, latch_pin;
    char user_input[9]; // 8 bites bináris adat + 1 hely a lezáró nullának

    // GPIO chip és pinek bekérése
    printf("Adja meg a GPIO chip nevét (pl. gpiochip0): ");
    scanf("%31s", chip_name);

    printf("Adja meg az adat pin számát: ");
    scanf("%d", &data_pin);

    printf("Adja meg az órajel pin számát: ");
    scanf("%d", &clock_pin);

    printf("Adja meg a tároló pin számát: ");
    scanf("%d", &latch_pin);

    // GPIO chip és vonalak inicializálása
    struct gpiod_chip *chip = gpiod_chip_open_by_name(chip_name);
    if (!chip) {
        perror("Nem sikerült megnyitni a GPIO chipet");
        return 1;
    }

    struct gpiod_line *data_line = gpiod_chip_get_line(chip, data_pin);
    struct gpiod_line *clock_line = gpiod_chip_get_line(chip, clock_pin);
    struct gpiod_line *latch_line = gpiod_chip_get_line(chip, latch_pin);

    if (!data_line || !clock_line || !latch_line) {
        perror("Nem sikerült elérni a GPIO vonalakat");
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(data_line, "shift_register", 0) < 0 ||
        gpiod_line_request_output(clock_line, "shift_register", 0) < 0 ||
        gpiod_line_request_output(latch_line, "shift_register", 0) < 0) {
        perror("Nem sikerült kimeneti módra állítani a vonalakat");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        printf("Adjon meg egy 8-bites bináris számsort (pl. 10101010, vagy 'q' kilépéshez): ");
        scanf("%8s", user_input); // Max. 8 karakter olvasása

        if (strcmp(user_input, "q") == 0) {
            break;
        }

        // Validáció: csak bináris számokat fogad el
        int valid = 1;
        for (int i = 0; i < 8; i++) {
            if (user_input[i] != '0' && user_input[i] != '1') {
                valid = 0;
                break;
            }
        }

        if (!valid || strlen(user_input) != 8) {
            printf("Hiba: Csak 8-bites bináris számot adjon meg!\n");
            continue;
        }

        shift_out(data_line, clock_line, latch_line, user_input);
        printf("Az adat kiírásra került a shift registerre: %s\n", user_input);
    }

    // GPIO felszabadítása
    gpiod_chip_close(chip);
    printf("Program vége. GPIO felszabadítása megtörtént.\n");

    return 0;
}

