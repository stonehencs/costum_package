#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#define GPIO_BASE_DEFAULT 0x1f207800 // Alapértelmezett GPIO memóriacím
#define GPIO_BLOCK_SIZE 4096         // Alapértelmezett blokk méret

void debug_error(const char *msg) {
    perror(msg);
    fprintf(stderr, "Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

int main() {
    uint32_t gpio_base;
    size_t block_size;
    int gpio_pin;

    printf("Adja meg a GPIO memóriacímét (hexadecimális, pl. 0x1f207800): ");
    if (scanf("%x", &gpio_base) != 1) {
        fprintf(stderr, "Hibás memóriacím formátum!\n");
        return EXIT_FAILURE;
    }

    printf("Adja meg a memória méretét (bájtokban, pl. 4096): ");
    if (scanf("%zu", &block_size) != 1) {
        fprintf(stderr, "Hibás memória méret formátum!\n");
        return EXIT_FAILURE;
    }

    printf("Adja meg a GPIO pin számát (0-tól kezdődően): ");
    if (scanf("%d", &gpio_pin) != 1) {
        fprintf(stderr, "Hibás GPIO pin formátum!\n");
        return EXIT_FAILURE;
    }

    // /dev/mem megnyitása
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        debug_error("Nem sikerült megnyitni a /dev/mem fájlt");
    }

    // Memória leképezése
    void *gpio_map = mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, gpio_base);
    if (gpio_map == MAP_FAILED) {
        debug_error("Nem sikerült leképezni a GPIO memóriát");
    }

    close(mem_fd);

    volatile uint32_t *gpio_addr = (volatile uint32_t *)gpio_map;

    // Teszt: GPIO pin írás
    uint32_t reg_offset = gpio_pin / 32; // Számoljuk ki, melyik regiszter érintett
    uint32_t bit_pos = gpio_pin % 32;   // Számoljuk ki a bit pozíciót

    printf("GPIO vezérlő regiszter írás teszt\n");

    // Alapregiszter olvasás
    uint32_t reg_value = gpio_addr[reg_offset];
    printf("Eredeti regiszter érték: 0x%08x\n", reg_value);

    // Pin magasra állítása
    reg_value |= (1 << bit_pos);
    gpio_addr[reg_offset] = reg_value;
    printf("Magasra állított regiszter érték: 0x%08x\n", gpio_addr[reg_offset]);

    // Pin alacsonyra állítása
    reg_value &= ~(1 << bit_pos);
    gpio_addr[reg_offset] = reg_value;
    printf("Alacsonyra állított regiszter érték: 0x%08x\n", gpio_addr[reg_offset]);

    // Memória terület felszabadítása
    if (munmap(gpio_map, block_size) < 0) {
        debug_error("Nem sikerült felszabadítani a GPIO memóriát");
    }

    printf("GPIO művelet sikeresen befejeződött.\n");
    return EXIT_SUCCESS;
}

