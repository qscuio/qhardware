# ESP-Hosted Wi-Fi station

Verification state: `HARDWARE_VERIFIED`

## Objective

Use ESP-Hosted over SDIO to initialize the onboard ESP32-C6, associate with a WPA2 or WPA3 access point, obtain a DHCP lease, and report disconnect reason codes without printing the password.

## Hardware

- Waveshare ESP32-P4-WIFI6 with its onboard ESP32-C6-MINI-1
- A reachable 2.4 GHz or 5 GHz access point supported by the C6 firmware

No external wiring is required beyond the USB data cable and antenna area kept clear of metal.

## Private configuration

Copy the example to the ignored local defaults file and edit only the copy:

```powershell
Copy-Item sdkconfig.defaults.example sdkconfig.defaults
```

```sh
cp sdkconfig.defaults.example sdkconfig.defaults
```

Replace `YOUR_SSID` and `YOUR_PASSWORD` locally. Never commit the resulting `sdkconfig.defaults`, generated `sdkconfig`, or serial logs containing private network details.

## Build and flash

Use the platform scripts with the ESP-IDF 5.5.5 path and the identified serial port. The component manager resolves ESP-Hosted 1.4.x and ESP Wi-Fi Remote 0.14.x.

```powershell
./scripts/build.ps1 -IdfPath C:\esp\esp-idf-v5.5.5
./scripts/flash.ps1 -IdfPath C:\esp\esp-idf-v5.5.5 -Port COM5
```

After reset, a pass requires the C6 capability event, Wi-Fi initialization, a DHCP address, and `connected to ap`. Disconnect reason 204 means handshake timeout; reason 205 is a general connection failure and should be interpreted with the preceding event.

## Recovery

If the SSID is not found, confirm band, channel, signal, and regulatory settings. If the handshake times out, re-enter the local password rather than printing it. Delete generated `sdkconfig` after changing Kconfig defaults so the next configure step imports the new values.
