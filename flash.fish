#!/usr/local/bin/fish
# Flashes program and prints size. Note: This is a shitty hack designed to be called from a cmake script, so it may not
# work normally
if set PROG_PORT (pavr2cmd --prog-port)
    avrdude -c stk500v2 -p atmega324pa -B 0.5 -P $PROG_PORT -F -U flash:w:build/assignment2.hex:i
    # print size
    avr-size -C --mcu=atmega324a build/assignment2.elf
end