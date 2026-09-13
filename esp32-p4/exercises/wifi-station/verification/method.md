# Verification method

1. Create the ignored `sdkconfig.defaults` from the committed example and enter local credentials.
2. Build and flash with ESP-IDF 5.5.5.
3. Reset the board and capture serial output at 115200 baud.
4. Confirm SDIO starts at 40 MHz and the C6 reports WLAN capability.
5. Confirm Wi-Fi initialization, DHCP, and the final association line.
6. Redact the assigned address, network name, MAC addresses, and all machine paths before saving evidence.

The password must never appear in build or runtime output.
