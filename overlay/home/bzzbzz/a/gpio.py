import gpiod
import time

def main():
    chip_name = input("Adja meg a GPIO chip nevét (pl. gpiochip0): ")
    line_offset = int(input("Adja meg a GPIO pin számát: "))

    try:
        # GPIO chip megnyitása
        chip = gpiod.Chip(chip_name)
    except OSError as e:
        print(f"Nem sikerült megnyitni a GPIO chipet: {e}")
        return

    try:
        # GPIO vonal (pin) lekérése
        line = chip.get_line(line_offset)
    except KeyError:
        print("Nem sikerült elérni a GPIO pint.")
        return

    try:
        # GPIO vonal kimeneti módra állítása
        line.request(consumer="gpio_control", type=gpiod.LINE_REQ_DIR_OUT)
    except OSError as e:
        print(f"Nem sikerült kimeneti módra állítani a pint: {e}")
        return

    try:
        # Pin magas szintre állítása
        print("Pin HIGH állapotba állítása")
        line.set_value(1)
        time.sleep(2)  # 2 másodperc várakozás

        # Pin alacsony szintre állítása
        print("Pin LOW állapotba állítása")
        line.set_value(0)
    except OSError as e:
        print(f"Hiba történt a GPIO állapotának beállításakor: {e}")
    finally:
        # GPIO felszabadítása
        line.release()
        print("GPIO művelet véget ért.")

if __name__ == "__main__":
    main()

