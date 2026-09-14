import asyncio
from bleak import BleakScanner


TARGET = "QHW-ESP32S3-PROBE"


async def main() -> int:
    found = asyncio.Event()
    result: dict[str, object] = {}

    def on_detection(device, advertisement_data) -> None:
        name = advertisement_data.local_name or device.name
        if name == TARGET:
            result["name"] = name
            result["rssi"] = advertisement_data.rssi
            found.set()

    async with BleakScanner(on_detection):
        try:
            await asyncio.wait_for(found.wait(), timeout=15)
        except TimeoutError:
            print(f"WINDOWS BLE SCAN FAIL: {TARGET} not received within 15 seconds")
            return 1

    print(f"WINDOWS BLE SCAN PASS: name={result['name']} rssi={result['rssi']}dBm")
    return 0


raise SystemExit(asyncio.run(main()))
