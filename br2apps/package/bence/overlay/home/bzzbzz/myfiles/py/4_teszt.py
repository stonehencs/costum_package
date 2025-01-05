import gpiod
import time

def setup_gpio(chip_name, pins):
    """GPIO lábak inicializálása."""
    chip = gpiod.Chip(chip_name)
    lines = {name: chip.get_line(pin) for name, pin in pins.items()}

    for line in lines.values():
        line.request(consumer="shift_register", type=gpiod.LINE_REQ_DIR_OUT)

    return lines

def shift_out(lines, data):
    """
    Shift registerre ír egy 8-bites adatot.
    :param lines: A GPIO lábak.
    :param data: 8-bites bináris adatsor.
    """
    data_pin = lines['data']
    clock_pin = lines['clock']

    for bit in data:
        data_pin.set_value(int(bit))  # Adat beállítása
        clock_pin.set_value(1)       # Órajel impulzus
        clock_pin.set_value(0)
    
def main():
    chip_name = input("Adja meg a GPIO chip nevét (pl. gpiochip0): ")
    data_pin = int(input("Adja meg az adat pin számát: "))
    clock_pin = int(input("Adja meg az órajel pin számát: "))
    latch_pin = int(input("Adja meg a tároló pin számát: "))
    output_enable_pin = int(input("Adja meg az output enable pin számát: "))
    delay_us = int(input("Adja meg a késleltetést két művelet között (μs): ")) / 1000000.0

    # GPIO lábak inicializálása
    pins = {"data": data_pin, "clock": clock_pin, "latch": latch_pin, "output_enable": output_enable_pin}
    lines = setup_gpio(chip_name, pins)

    # Adatok beolvasása a CSV fájlból
    try:
        with open("data.txt", "r") as file:
            data_lines = file.readlines()
    except FileNotFoundError:
        print("Hiba: A 'data.txt' fájl nem található.")
        return

    try:
        for line in data_lines:
            line = line.strip()  # Sorvégi whitespace eltávolítása
            if len(line) != 200:  # 25 x 8 bites adatsor ellenőrzése
                print(f"Hiba: Rossz adatsor hossza (várható: 200 karakter, kapott: {len(line)}).")
                continue
            
            # Minden 8 bites blokk kiírása
            lines['output_enable'].set_value(1)  # Output Enable HIGH
            lines['latch'].set_value(0)
            
            for i in range(25):
                block = line[i*8:(i+1)*8]  # Egy 8-bites blokk
                shift_out(lines, block)
            
            lines['latch'].set_value(1)  # Latch adatok
            time.sleep(0.00000001)
            lines['latch'].set_value(0)
            lines['output_enable'].set_value(0)  # Output Enable LOW
            
            # Mikroszekundumos késleltetés két művelet között
            time.sleep(delay_us)
    finally:
        # GPIO felszabadítása
        for line in lines.values():
            line.release()
        print("GPIO felszabadítása megtörtént.")

if __name__ == "__main__":
    main()

