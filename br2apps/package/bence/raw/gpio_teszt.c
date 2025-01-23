#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define GPIO_BASE_ADDR 0x1f207800  // Az alapértelmezett GPIO bázis cím
#define GPIO_OFFSET 0x10          // Az egyes GPIO-k közötti távolság bájtokban

// Makrók a bitekhez
#define GPIO_OEN_BIT 5
#define GPIO_OUT_BIT 4
#define GPIO_IN_BIT 0

volatile uint32_t *gpio_base = NULL;
size_t gpio_block_size = 0;

int init_gpio_memory(off_t base_addr, size_t block_size) {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Hiba a /dev/mem megnyitásakor");
        return -1;
    }

    gpio_base = mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, base_addr);
    close(mem_fd);

    if (gpio_base == MAP_FAILED) {
        perror("Hiba a memória leképezésekor");
        gpio_base = NULL;
        return -1;
    }

    gpio_block_size = block_size;
    return 0;
}

void cleanup_gpio_memory() {
    if (gpio_base) {
        munmap((void *)gpio_base, gpio_block_size);
        gpio_base = NULL;
    }
}

void set_gpio_mode(int gpio, int mode) {
    if (!gpio_base) {
        printf("A GPIO memória nem inicializált.\n");
        return;
    }

    volatile uint32_t *reg = gpio_base + gpio;
    uint32_t value = *reg;

    if (mode == 0) { // Kimeneti mód
        value &= ~(1 << GPIO_OEN_BIT); // ~OEN = 0
    } else { // Bemeneti mód
        value |= (1 << GPIO_OEN_BIT);  // ~OEN = 1
    }

    *reg = value;
    printf("GPIO %d módja: %s\n", gpio, mode ? "Bemenet" : "Kimenet");
}

void set_gpio_output(int gpio, int value) {
    if (!gpio_base) {
        printf("A GPIO memória nem inicializált.\n");
        return;
    }

    volatile uint32_t *reg = gpio_base + gpio;
    uint32_t reg_value = *reg;

    if (value) {
        reg_value |= (1 << GPIO_OUT_BIT);  // OUT = 1
    } else {
        reg_value &= ~(1 << GPIO_OUT_BIT); // OUT = 0
    }

    *reg = reg_value;
    printf("GPIO %d kimeneti értéke: %d\n", gpio, value);
}

int read_gpio_input(int gpio) {
    if (!gpio_base) {
        printf("A GPIO memória nem inicializált.\n");
        return -1;
    }

    volatile uint32_t *reg = gpio_base + gpio;
    uint32_t reg_value = *reg;

    int value = (reg_value >> GPIO_IN_BIT) & 1; // Az IN bit kiolvasása
    printf("GPIO %d bemeneti értéke: %d\n", gpio, value);
    return value;
}

int main() {
    int gpio;
    int mode;
    int value;
    size_t block_size;

    printf("Add meg a GPIO blokk méretét (GPIO_BLOCK_SIZE) 4096: ");
    if (scanf("%zu", &block_size) != 1 || block_size <= 0) {
        printf("Hibás blokk méret!\n");
        return 1;
    }

    if (init_gpio_memory(GPIO_BASE_ADDR, block_size) != 0) {
        printf("Nem sikerült inicializálni a GPIO memóriát.\n");
        return 1;
    }

    printf("Add meg a GPIO számát (pl. 12): ");
    if (scanf("%d", &gpio) != 1 || gpio < 0) {
        printf("Hibás GPIO szám!\n");
        cleanup_gpio_memory();
        return 1;
    }

    printf("Válaszd ki a módot (0: kimenet, 1: bemenet): ");
    if (scanf("%d", &mode) != 1 || (mode != 0 && mode != 1)) {
        printf("Hibás mód!\n");
        cleanup_gpio_memory();
        return 1;
    }

    set_gpio_mode(gpio, mode);

    if (mode == 0) {
        printf("Add meg a kimeneti értéket (0 vagy 1): ");
        if (scanf("%d", &value) != 1 || (value != 0 && value != 1)) {
            printf("Hibás érték!\n");
            cleanup_gpio_memory();
            return 1;
        }
        set_gpio_output(gpio, value);
    } else {
        read_gpio_input(gpio);
    }

    cleanup_gpio_memory();
    return 0;
}

