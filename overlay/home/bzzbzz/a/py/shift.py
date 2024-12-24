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
    :param data: 8-bites számsor, amit a shift registerre kell írni.
    """
    data_pin = lines['data']
    clock_pin = lines['clock']
    latch_pin = lines['latch']

    # Latch reset (zárva tartás adatírás közben)
    latch_pin.set_value(0)
    time.sleep(0.001)

    # Bitenkénti adatkiírás
    for bit in data:
        data_pin.set_value(int(bit))  # Adat beállítása
        time.sleep(0.001)            # Rövid késleltetés
        clock_pin.set_value(1)       # Órajel impulzus
        time.sleep(0.001)
        clock_pin.set_value(0)
    
    # Latch beállítása (adat érvényesítése)
    latch_pin.set_value(1)
    time.sleep(0.001)
    latch_pin.set_value(0)

def main():
    chip_name = input("Adja meg a GPIO chip nevét (pl. gpiochip0): ")
    data_pin = int(input("Adja meg az adat pin számát: "))
    clock_pin = int(input("Adja meg az órajel pin számát: "))
    latch_pin = int(input("Adja meg a tároló pin számát: "))

    # GPIO lábak inicializálása
    pins = {"data": data_pin, "clock": clock_pin, "latch": latch_pin}
    lines = setup_gpio(chip_name, pins)

    while True:
        user_input = input("Adjon meg egy 8-bites bináris számsort (pl. 10101010, vagy 'q' kilépéshez): ")
        if user_input.lower() == 'q':
            break
        if len(user_input) != 8 or not all(c in '01' for c in user_input):
            print("Hiba: Csak 8-bites bináris számot adjon meg!")
            continue
        
        shift_out(lines, user_input)
        print(f"Az adat kiírásra került a shift registerre: {user_input}")

    print("Program vége. GPIO felszabadítása.")
    for line in lines.values():
        line.release()

if __name__ == "__main__":
    main()

