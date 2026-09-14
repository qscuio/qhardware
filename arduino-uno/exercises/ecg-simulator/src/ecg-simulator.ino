// ECG-like waveform generator for Arduino Uno.
// D3: 62.5 kHz PWM carrying the analog waveform.
// D6: approximately 976 Hz, 50% duty-cycle filter test signal.
// D8: digital trigger pulse aligned with the R peak.

const uint8_t ECG_PIN = 3;
const uint8_t FILTER_TEST_PIN = 6;
const uint8_t TRIGGER_PIN = 8;
const uint16_t BEAT_PERIOD_MS = 833;  // Approximately 72 BPM.

int16_t parabolicPulse(uint16_t timeMs, uint16_t centerMs,
                       uint16_t halfWidthMs, int16_t amplitude) {
  const int16_t distance = abs((int16_t)timeMs - (int16_t)centerMs);
  if (distance >= (int16_t)halfWidthMs) {
    return 0;
  }

  const int32_t widthSquared = (int32_t)halfWidthMs * halfWidthMs;
  const int32_t distanceSquared = (int32_t)distance * distance;
  return (int32_t)amplitude * (widthSquared - distanceSquared) /
         widthSquared;
}

int16_t trianglePulse(uint16_t timeMs, uint16_t centerMs,
                      uint16_t halfWidthMs, int16_t amplitude) {
  const int16_t distance = abs((int16_t)timeMs - (int16_t)centerMs);
  if (distance >= (int16_t)halfWidthMs) {
    return 0;
  }

  return (int32_t)amplitude * (halfWidthMs - distance) / halfWidthMs;
}

void setup() {
  pinMode(ECG_PIN, OUTPUT);
  pinMode(FILTER_TEST_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  // Timer0 already drives pin 6 at approximately 976 Hz on the Uno.
  analogWrite(FILTER_TEST_PIN, 128);

  // Configure Timer2 explicitly for fast PWM on OC2B (pin 3).
  // 16 MHz / 256 = 62.5 kHz, which is easier to remove with an RC filter.
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  OCR2B = 100;
}

void loop() {
  static unsigned long nextSampleUs = 0;
  const unsigned long nowUs = micros();
  if ((long)(nowUs - nextSampleUs) < 0) {
    return;
  }
  nextSampleUs = nowUs + 1000UL;  // Update the waveform at 1 kHz.

  const unsigned long nowMs = millis();
  const uint16_t phase = nowMs % BEAT_PERIOD_MS;

  // Baseline plus P wave, QRS complex, and T wave.
  int16_t level = 100;
  level += parabolicPulse(phase, 120, 70, 15);    // P
  level += trianglePulse(phase, 235, 18, -24);   // Q
  level += trianglePulse(phase, 260, 14, 115);   // R
  level += trianglePulse(phase, 285, 24, -40);   // S
  level += parabolicPulse(phase, 470, 130, 28);  // T

  // Add a subtle five-second baseline drift, similar to respiration.
  const uint16_t breath = nowMs % 5000UL;
  level += breath < 2500U
      ? -4 + (int32_t)breath * 8 / 2500
      : 4 - (int32_t)(breath - 2500U) * 8 / 2500;

  level = constrain(level, 0, 255);
  analogWrite(ECG_PIN, level);

  digitalWrite(TRIGGER_PIN, phase >= 252U && phase < 269U ? HIGH : LOW);
}
