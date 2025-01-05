#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <string.h>

#define BITS_IN_BYTE 8
#define TOTAL_BLOCKS 25
#define TOTAL_BITS (BITS_IN_BYTE * TOTAL_BLOCKS)

void shift_out(struct gpiod_line *data, struct gpiod_line *clock, struct gpiod_line *latch, const char *bitstring) {

	
    // Minden bitet shiftelünk
    for (int i = 0; i < BITS_IN_BYTE; i++) {
        int bit = bitstring[i] - '0'; // Konvertáljuk 1/0 értékké
        gpiod_line_set_value(data, bit); // Beállítjuk az adatot
        gpiod_line_set_value(clock, 1); // Clock HIGH
        gpiod_line_set_value(clock, 0); // Clock LOW
    }

}

int main() {
    char chip_name[32];
    int data_pin, clock_pin, latch_pin, output_enable_pin;
    unsigned int delay_us;
    char file_name[256]; // Fájlnév maximális hossza

    printf("Adja meg a GPIO chip nevét (pl. gpiochip0): ");
    scanf("%31s", chip_name);

    printf("Adja meg az adat pin számát: ");
    scanf("%d", &data_pin);

    printf("Adja meg az órajel pin számát: ");
    scanf("%d", &clock_pin);

    printf("Adja meg a tároló pin számát: ");
    scanf("%d", &latch_pin);

    printf("Adja meg az output enable pin számát: ");
    scanf("%d", &output_enable_pin);

    printf("Adja meg a késleltetést két művelet között (μs): ");
    scanf("%u", &delay_us);

    printf("Adja meg az adatok fájl nevét vagy elérési útját: ");
    scanf("%255s", file_name); // Max 255 karakteres fájlútvonal

    // GPIO inicializálása
    struct gpiod_chip *chip = gpiod_chip_open_by_name(chip_name);
    if (!chip) {
        perror("Nem sikerült megnyitni a GPIO chipet");
        return 1;
    }

    struct gpiod_line *data = gpiod_chip_get_line(chip, data_pin);
    struct gpiod_line *clock = gpiod_chip_get_line(chip, clock_pin);
    struct gpiod_line *latch = gpiod_chip_get_line(chip, latch_pin);
    struct gpiod_line *output_enable = gpiod_chip_get_line(chip, output_enable_pin);

    if (!data || !clock || !latch || !output_enable) {
        perror("Nem sikerült elérni a GPIO lábakat");
        gpiod_chip_close(chip);
        return 1;
    }

    // GPIO vonalak kimeneti módra állítása
    gpiod_line_request_output(data, "shift_register", 0);
    gpiod_line_request_output(clock, "shift_register", 0);
    gpiod_line_request_output(latch, "shift_register", 0);
    gpiod_line_request_output(output_enable, "shift_register", 0);

    // Fájl beolvasása
    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("Nem található a megadott fájl");
        gpiod_chip_close(chip);
        return 1;
    }

    char line[TOTAL_BITS + 1]; // 25x8 = 200 + 1 a null terminátor számára

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // Sorvége karakter eltávolítása

        // Minden blokk (25 darab 8 bites adat) feldolgozása
        gpiod_line_set_value(output_enable, 1); // Output Enable HIGH
		
        for (int i = 0; i < TOTAL_BLOCKS; i++) {
            shift_out(data, clock, latch, &line[i * BITS_IN_BYTE]);
        }
		gpiod_line_set_value(latch, 1);
		gpiod_line_set_value(latch, 0);
		
        gpiod_line_set_value(output_enable, 0); // Output Enable LOW

        // Mikroszekundumos késleltetés
        usleep(delay_us);
    }

    fclose(file);

    // GPIO felszabadítása
    gpiod_chip_close(chip);
    printf("GPIO műveletek befejezve.\n");

    return 0;
}

