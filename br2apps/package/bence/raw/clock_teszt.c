#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

#define GPIO_BASE 0x1f207800 // GPIO alapcím (gpiochip2)
#define BLOCK_SIZE 4096      // Memória blokkméret

volatile uint32_t *gpio; // GPIO regiszter pointer

// GPIO inicializáció (memória leképezése)
void setup_gpio() {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Nem sikerült megnyitni a /dev/mem fájlt");
        exit(1);
    }

    gpio = (volatile uint32_t *)mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE
    );

    if (gpio == MAP_FAILED) {
        perror("Nem sikerült leképezni a GPIO memóriát");
        close(mem_fd);
        exit(1);
    }

    close(mem_fd);
}

// GPIO láb kimeneti módra állítása
void set_gpio_output(int line) {
    int reg_index = line / 10;       // GPFSEL regiszter kiválasztása
    int bit_offset = (line % 10) * 3; // Pinhez tartozó bitek

    gpio[reg_index] &= ~(7 << bit_offset); // Bitek törlése
    gpio[reg_index] |= (1 << bit_offset);  // Kimeneti mód beállítása
}

// GPIO láb értékének beállítása (HIGH vagy LOW)
void write_gpio(int line, int value) {
    int reg_index = (value ? 7 : 10); // GPSET vagy GPCLR regiszter
    gpio[reg_index] = (1 << line);    // Pin beállítása
}

// Gyors clock jel generálása
void generate_clock(int line, int cycles) {
    for (int i = 0; i < cycles; i++) {
        write_gpio(line, 1); // HIGH
        write_gpio(line, 0); // LOW
    }
}

int main() {
    int clock_line = 10; // GPIO chip 2, line 10
    int cycles = 1000000; // A clock ciklusainak száma

    // GPIO inicializálása
    setup_gpio();

    // GPIO láb kimeneti módra állítása
    set_gpio_output(clock_line);

    // Clock jel generálása
    generate_clock(clock_line, cycles);

    printf("Clock jel generálása befejeződött.\n");

    return 0;
}

