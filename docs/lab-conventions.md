# Lab conventions

## Work safely

Disconnect power before changing wiring. Confirm a peripheral's supply and logic voltage from its datasheet or board markings before connecting it. All connected boards must share ground. Never join exposed conductors by hand while power is applied, and never power motors, servos, speakers, or other substantial loads from a GPIO pin.

Identify the exact serial port, network host, or block device immediately before a flash, erase, restore, or imaging command. The provided scripts stop on command failures and do not erase full devices automatically.

## Keep runs reproducible

Commands use parameters such as `PORT`, `IDF_PATH`, `PI_HOST`, and output directories. Do not replace them in committed files with paths or addresses from one computer. Record tool versions, target board, command, date, and relevant physical connections.

Pin tables use the signal names printed on the tested board. Add a note when an Arduino pin number, SoC GPIO number, header position, or connector label might be confused with another numbering scheme.

## Keep private and generated data local

Do not commit Wi-Fi credentials, reusable authentication keys, API tokens, private SSH keys, private host inventories, or unredacted private IP addresses. Keep SDK downloads, dependency caches, build directories, device backups, and operating-system images outside Git. Use example configuration files with values such as `YOUR_SSID`, then copy them to an ignored local file.

Vendor assets are linked to their source unless redistribution terms clearly allow copying. A checksum identifies an exact input or generated artifact; it does not grant redistribution rights.
