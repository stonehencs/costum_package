import gpiod
import time
import random
import threading

# GPIO beállítások
CHIP_NAME = "gpiochip0"
LATCH_PIN = 2
BLANK_PIN = 4
DATA_PIN = 11
CLOCK_PIN = 13

# GPIO inicializáció
def init_gpio():
    try:
        chip = gpiod.Chip(CHIP_NAME)
        latch_line = chip.get_line(LATCH_PIN)
        blank_line = chip.get_line(BLANK_PIN)
        data_line = chip.get_line(DATA_PIN)
        clock_line = chip.get_line(CLOCK_PIN)

        latch_line.request(consumer="LED_Cube", type=gpiod.LINE_REQ_DIR_OUT)
        blank_line.request(consumer="LED_Cube", type=gpiod.LINE_REQ_DIR_OUT)
        data_line.request(consumer="LED_Cube", type=gpiod.LINE_REQ_DIR_OUT)
        clock_line.request(consumer="LED_Cube", type=gpiod.LINE_REQ_DIR_OUT)

        return chip, latch_line, blank_line, data_line, clock_line
    except OSError as e:
        print(f"Nem sikerült inicializálni a GPIO-t: {e}")
        return None, None, None, None, None

# Shift regiszter adatküldés (MSBFIRST)
def shift_out(byte, data_line, clock_line):
    for bit in range(8):
        data_line.set_value((byte >> (7 - bit)) & 1)  # MSBFIRST
        clock_line.set_value(1)
        clock_line.set_value(0)

# Multiplexálás és BAM
def multiplex_bam(latch_line, blank_line, data_line, clock_line, red, green, blue):
    bam_bit = 0
    bam_counter = 0
    anode = [0b00000001 << i for i in range(8)]  # 8 szint

    while True:
        # Kikapcsolás
        blank_line.set_value(1)
        
        # BAM bit kezelés
        if bam_counter == 8:
            bam_bit += 1
        elif bam_counter == 24:
            bam_bit += 1
        elif bam_counter == 56:
            bam_bit += 1
        if bam_counter >= 120:
            bam_bit = 0
            bam_counter = 0
        bam_counter += 1

        # Színek küldése SPI-n keresztül
        for i in range(8):
            shift_out(red[bam_bit][i], data_line, clock_line)  # Piros
            shift_out(green[bam_bit][i], data_line, clock_line)  # Zöld
            shift_out(blue[bam_bit][i], data_line, clock_line)  # Kék

        # Anód frissítés
        shift_out(anode[bam_counter % 8], data_line, clock_line)

        latch_line.set_value(1)
        latch_line.set_value(0)
        blank_line.set_value(0)
        time.sleep(0.001)  # Multiplex frekvencia

# LED-ek adatstruktúrája
def initialize_led_array():
    return [[[0] * 8 for _ in range(8)] for _ in range(4)]  # 4 BAM szint, 8x8 mátrix

# LED állítás
def set_led(red, green, blue, level, row, column, r, g, b):
    for bit in range(4):  # BAM szintek
        red[bit][row][column] = (r >> bit) & 1
        green[bit][row][column] = (g >> bit) & 1
        blue[bit][row][column] = (b >> bit) & 1

# Animációs funkciók
def rain_animation(red, green, blue):
    """Eső animáció."""
    for _ in range(100):  # Véletlenszerű LED-ek bekapcsolása
        level, row, column = (random.randint(0, 7) for _ in range(3))
        set_led(red, green, blue, level, row, column, random.randint(1, 15), 0, random.randint(1, 15))
        time.sleep(0.1)

def sinewave_animation(red, green, blue):
    """Színusz hullám animáció."""
    sinewave = [0, 1, 2, 3, 4, 5, 6, 7]
    directions = [1] * 8
    for _ in range(150):
        for i in range(8):
            sinewave[i] += directions[i]
            if sinewave[i] == 7:
                directions[i] = -1
            elif sinewave[i] == 0:
                directions[i] = 1
            set_led(red, green, blue, 0, i, sinewave[i], 15, 15, 0)
        time.sleep(0.1)

def color_wheel_animation(red, green, blue):
    """Színes kerék animáció."""
    for _ in range(100):
        for level in range(8):
            for row in range(8):
                for column in range(8):
                    set_led(red, green, blue, level, row, column, random.randint(0, 15), random.randint(0, 15), random.randint(0, 15))
        time.sleep(0.05)

def bouncy_ball_animation(red, green, blue):
    """Pattogó labda animáció."""
    x, y, z = 4, 4, 4
    dx, dy, dz = 1, 1, 1
    for _ in range(100):
        set_led(red, green, blue, z, y, x, 15, 0, 0)
        time.sleep(0.1)
        x += dx
        y += dy
        z += dz
        if x == 0 or x == 7:
            dx *= -1
        if y == 0 or y == 7:
            dy *= -1
        if z == 0 or z == 7:
            dz *= -1

def wipe_out_animation(red, green, blue):
    """Wipe out animáció."""
    for level in range(8):
        for row in range(8):
            for column in range(8):
                set_led(red, green, blue, level, row, column, random.randint(0, 15), random.randint(0, 15), random.randint(0, 15))
                time.sleep(0.02)
                set_led(red, green, blue, level, row, column, 0, 0, 0)

    for level in range(7, -1, -1):
        for row in range(7, -1, -1):
            for column in range(7, -1, -1):
                set_led(red, green, blue, level, row, column, 0, 0, 0)
                time.sleep(0.02)

# Főprogram
if __name__ == "__main__":
    chip, latch_line, blank_line, data_line, clock_line = init_gpio()
    if not chip:
        exit(1)

    # LED mátrix inicializálás
    red = initialize_led_array()
    green = initialize_led_array()
    blue = initialize_led_array()

    try:
        # Multiplex és animáció futtatása párhuzamosan
        multiplex_thread = threading.Thread(
            target=multiplex_bam, args=(latch_line, blank_line, data_line, clock_line, red, green, blue), daemon=True
        )
        multiplex_thread.start()

        # Animációk sorrendje
        animations = [
            rain_animation,
            sinewave_animation,
            color_wheel_animation,
            bouncy_ball_animation,
            wipe_out_animation,  # ÚJ wipe out animáció
        ]
        for animation in animations:
            animation(red, green, blue)

    except KeyboardInterrupt:
        print("Program leállítva.")
    finally:
        # GPIO felszabadítása
        latch_line.release()
        blank_line.release()
        data_line.release()
        clock_line.release()
        print("GPIO vonalak felszabadítva.")

