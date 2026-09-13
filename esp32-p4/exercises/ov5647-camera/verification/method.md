# Verification method

1. Power down and connect the OV5647 with the purpose-built Waveshare adapter ribbon.
2. Build with ESP-IDF 5.5.5, flash the application, and open the port at 115200 baud.
3. Reset the board and capture the log from the ROM banner through the first frame.
4. Confirm the actual chip revision is 3.2 and the sensor driver reports PID `0x5647`.
5. Confirm the processed frame is 800 by 800 RGB565 and contains 1,280,000 bytes.
6. Confirm `CAMERA TEST PASS` appears and the task remains alive.

If a second capture is used to test live image change, move an object or change lighting and confirm the sampled checksum changes. A checksum change supplements the frame-size check; it does not evaluate image quality.
