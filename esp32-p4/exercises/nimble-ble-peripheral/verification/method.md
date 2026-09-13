# Verification method

1. Build and flash with ESP-IDF 5.5.5.
2. Reset and capture the serial log through the first GAP advertising event.
3. Confirm the C6 reports HCI over SDIO and BLE-only capability.
4. Confirm hosted Bluetooth support is enabled with VHCI transport.
5. Confirm the NimBLE host task starts and advertising begins without an error return.
6. Turn on a nearby BLE scanner and search for `nimble-bleprph` for at least 20 seconds.
7. Record serial and scanner results separately; redact device addresses and nearby device names.

The serial criteria verify the physical P4-to-C6 hosted controller path. Scanner discovery additionally verifies over-the-air transmission and the scanner environment.
