#include <stdio.h>
#include <gpiod.h>
#include <unistd.h> // usleep függvényhez

#define HIGH 1
#define LOW  0

int main() {
    char chip_name[32];
    int pin;
    unsigned int delay_us;

    printf("Adja meg a GPIO chip nevét (pl. gpiochip0): ");
    scanf("%31s", chip_name);

    printf("Adja meg a GPIO pin számát: ");
    scanf("%d", &pin);

    printf("Adja meg a blinkelés késleltetését (μs): ");
    scanf("%u", &delay_us);

    // GPIO chip inicializálása
    struct gpiod_chip *chip = gpiod_chip_open_by_name(chip_name);
    if (!chip) {
        perror("Nem sikerült megnyitni a GPIO chipet");
        return 1;
    }

    // GPIO pin elérése
    struct gpiod_line *line = gpiod_chip_get_line(chip, pin);
    if (!line) {
        perror("Nem sikerült elérni a GPIO pint");
        gpiod_chip_close(chip);
        return 1;
    }

    // GPIO pin kimeneti módra állítása
    if (gpiod_line_request_output(line, "blink", LOW) < 0) {
        perror("Nem sikerült kimeneti módra állítani a pint");
        gpiod_chip_close(chip);
        return 1;
    }

    printf("Blinkelés elkezdődik. Nyomjon Ctrl+C-t a leállításhoz.\n");

    // Végtelen ciklus blinkeléshez
    while (1) {
        gpiod_line_set_value(line, HIGH);
        gpiod_line_set_value(line, LOW);

    }

    // GPIO felszabadítása (ez a kód nem éri el a while végtelensége miatt)
    gpiod_chip_close(chip);
    return 0;
}

