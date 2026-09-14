# Debugging Arduino Uno

Start with the smallest layer that can fail.

## Board is absent

Try another known data cable and USB port. Compare `arduino-cli board list` before and after reconnecting. On Windows, inspect Device Manager for a warning icon; on Linux, inspect `dmesg --follow` while reconnecting and confirm group membership with `groups`.

## Upload fails

Close serial monitors and other programs using the port, confirm `--fqbn arduino:avr:uno`, and retry with verbose upload output:

```sh
arduino-cli upload -v -p PORT --fqbn arduino:avr:uno PATH_TO_SKETCH
```

`avrdude: ser_open()` usually means the port is wrong, busy, or inaccessible. Repeated `stk500_recv()` errors point to the wrong board/port, a blocked auto-reset path, pins 0/1 being driven, or a missing/corrupt bootloader.

For a USB-serial loopback test, remove external wiring, connect RESET to GND, connect RX to TX, open the serial terminal, and confirm typed characters echo. Remove both jumpers afterward. Some CH340 Uno variants do not support this test reliably.

## Sketch runs incorrectly

- Add bounded serial messages at setup milestones and around sensor reads.
- Check supply voltage at the load while it moves or transmits.
- Confirm a common ground and recheck the exercise wiring table.
- Read the compiler memory report. Near-full SRAM often causes resets or corrupt text.
- Check timer ownership. `Servo` uses Timer1; direct Timer1 code and some PWM pins can conflict. The ECG exercise configures Timer2.
- Reduce the system to one sensor or actuator, then add parts back one at a time.

The onboard LED blink sketch is the baseline test. If it compiles, uploads, and flashes predictably, the USB, bootloader, core, and basic board operation are working.
