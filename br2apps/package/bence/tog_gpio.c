#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

#define HIGH 1
#define LOW  0

int main() {
    char chip_name[32];
    int line_offset;

    printf("Adja meg a GPIO chip nevét (pl. gpiochip0): ");
    scanf("%31s", chip_name);

    printf("Adja meg a GPIO pin számát: ");
    scanf("%d", &line_offset);

    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int ret;

    // GPIO chip megnyitása
    chip = gpiod_chip_open_by_name(chip_name);
    if (!chip) {
        perror("Nem sikerült megnyitni a GPIO chipet");
        return 1;
    }

    // GPIO line kiválasztása
    line = gpiod_chip_get_line(chip, line_offset);
    if (!line) {
        perror("Nem sikerült elérni a GPIO pint");
        gpiod_chip_close(chip);
        return 1;
    }

    // GPIO line kimeneti módra állítása
    ret = gpiod_line_request_output(line, "gpiod_control", LOW);
    if (ret < 0) {
        perror("Nem sikerült kimeneti módra állítani a pint");
        gpiod_chip_close(chip);
        return 1;
    }

    // Pin magas szintre állítása
    printf("Pin HIGH állapotba állítása\n");
    ret = gpiod_line_set_value(line, HIGH);
    if (ret < 0) {
        perror("Nem sikerült HIGH állapotba állítani a pint");
        gpiod_chip_close(chip);
        return 1;
    }

    sleep(2); // Várakozás 2 másodpercig

    // Pin alacsony szintre állítása
    printf("Pin LOW állapotba állítása\n");
    ret = gpiod_line_set_value(line, LOW);
    if (ret < 0) {
        perror("Nem sikerült LOW állapotba állítani a pint");
        gpiod_chip_close(chip);
        return 1;
    }

    // GPIO chip bezárása
    gpiod_chip_close(chip);
    printf("GPIO művelet véget ért\n");

    return 0;
}

