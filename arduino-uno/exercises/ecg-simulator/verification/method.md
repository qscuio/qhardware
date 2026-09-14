# Verification method

1. Install Arduino AVR Boards 1.8.8 and the libraries declared in the README.
2. Compile the sketch for `arduino:avr:uno`.
3. Upload it to an Uno with power removed from external loads during reset.
4. Reconnect the documented hardware safely and perform the observable check below.
5. Record compiler output and, when hardware is present, the observed result without machine-specific paths.

Trigger the oscilloscope from D8, view filtered D3, and confirm a stable repeating complex with a period near 833 ms.
