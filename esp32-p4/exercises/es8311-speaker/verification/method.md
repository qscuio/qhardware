# Verification method

1. With power removed, connect the passive speaker to the two-pin SPK socket.
2. Build, flash, and monitor at 115200 baud.
3. Confirm the ES8311 speaker handle opens at 16 kHz, stereo, 16-bit, volume 60.
4. Listen for a repeating low-volume tone.
5. Confirm at least two complete 640,000-byte writes without an error or reset.

Serial write completion verifies the digital path. The listening step verifies the codec, amplifier, connector, and speaker together.
