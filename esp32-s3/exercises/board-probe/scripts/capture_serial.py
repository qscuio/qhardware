import argparse
import pathlib
import re
import time

import serial
from serial.tools import list_ports


parser = argparse.ArgumentParser()
parser.add_argument("--port", required=True)
parser.add_argument("--duration", type=float, default=20)
parser.add_argument("--output", type=pathlib.Path)
parser.add_argument(
    "--reset",
    action="store_true",
    help="pulse RTS after opening; omit when native USB would select download mode",
)
args = parser.parse_args()

PORT = args.port
OUTPUT = args.output or pathlib.Path(__file__).with_name("probe-serial.log")

native_usb = any(
    item.device.casefold() == PORT.casefold()
    and item.vid == 0x303A
    and item.pid == 0x1001
    for item in list_ports.comports()
)

# Configure inactive control lines before opening. Opening a native USB-JTAG
# port with pyserial's asserted defaults can select ROM download mode.
port = serial.Serial(port=None, baudrate=115200, timeout=0.1)
port.dtr = False
port.rts = False
port.port = PORT
port.open()
if args.reset:
    time.sleep(0.1)
    port.rts = True
    time.sleep(0.2 if native_usb else 0.1)
    port.rts = False
    if native_usb:
        time.sleep(0.2)

deadline = time.monotonic() + args.duration
chunks: list[bytes] = []
while time.monotonic() < deadline:
    data = port.read(port.in_waiting or 1)
    if data:
        chunks.append(data)
port.close()

text = b"".join(chunks).decode("utf-8", errors="replace")
text = re.sub(r"(?:[0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}", "[redacted]", text)
OUTPUT.write_text(text, encoding="utf-8")
for line in text.splitlines():
    if "QHW_PROBE:" in line or "2nd stage bootloader" in line:
        print(line)
