#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm(0x40);
const uint8_t channels[4] = {0, 1, 2, 3}; // base, shoulder, elbow, gripper
const uint16_t SERVO_MIN_US = 1000;
const uint16_t SERVO_MAX_US = 2000;

float decodeCentiDegrees(byte highByte, byte lowByte) {
  uint16_t centiDegrees = ((uint16_t)highByte << 8) | lowByte;
  return centiDegrees / 100.0f;
}

void writeAngle(uint8_t channel, float degrees) {
  degrees = constrain(degrees, 0.0f, 180.0f);
  uint16_t pulseUs = SERVO_MIN_US +
      (uint16_t)((SERVO_MAX_US - SERVO_MIN_US) * degrees / 180.0f);
  pwm.writeMicroseconds(channel, pulseUs);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!pwm.begin()) {
    Serial.println(F("ERROR:PCA9685_NOT_FOUND"));
    while (true) delay(1000);
  }
  pwm.setPWMFreq(50);
  delay(10);

  uint8_t position[4];
  for (uint8_t i = 0; i < 4; i++) {
    position[i] = EEPROM.read(i);
    if (position[i] > 180) position[i] = 90;
    writeAngle(channels[i], position[i]);
  }

  Serial.print(F("INIT:"));
  Serial.print(position[0]); Serial.print(',');
  Serial.print(position[1]); Serial.print(',');
  Serial.print(position[2]); Serial.print(',');
  Serial.println(position[3]);
}

void loop() {
  if (Serial.available() < 8) return;

  float angles[4];
  for (uint8_t i = 0; i < 4; i++) {
    byte highByte = Serial.read();
    byte lowByte = Serial.read();
    angles[i] = decodeCentiDegrees(highByte, lowByte);
  }

  for (uint8_t i = 0; i < 4; i++) {
    if (angles[i] < 0.0f || angles[i] > 180.0f) return;
  }

  for (uint8_t i = 0; i < 4; i++) {
    writeAngle(channels[i], angles[i]);
    EEPROM.update(i, (uint8_t)round(angles[i]));
  }
}
