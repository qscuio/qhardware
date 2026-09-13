# Onboard LED blink

Verification state: `BUILD_VERIFIED`

## Objective

Verify the Uno toolchain, bootloader, and main loop by flashing the onboard LED in a three-pulse pattern.

## Parts

- Arduino Uno and USB data cable

## Wiring

| Signal | Uno pin |\n| --- | --- |\n| Onboard LED | D13 (`LED_BUILTIN`) |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

No external library is required.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/blink
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/blink` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

The onboard LED flashes three times at 120 ms on/off intervals, then remains off for about 1.2 seconds before repeating.

## Validation

Observe at least three complete groups and confirm each group contains exactly three short flashes followed by a longer pause.

## Recovery

If the LED stays fixed, upload again with external wiring removed and use the upload diagnostics in `../../docs/debugging.md`.
