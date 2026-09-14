#include <LiquidCrystal.h>

const int LCD_RS = 12;
const int LCD_ENABLE = 11;
const int LCD_D4 = 5;
const int LCD_D5 = 4;
const int LCD_D6 = 3;
const int LCD_D7 = 2;

const int PIR_PIN = 7;

LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

bool lastMotion = false;
unsigned int motionCount = 0;

void showStatus(bool motion) {
  lcd.setCursor(0, 0);
  lcd.print("PIR: ");
  lcd.print(motion ? "Motion " : "Idle   ");

  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(motionCount);
  lcd.print("       ");

  Serial.print("pir=");
  Serial.print(motion ? "motion" : "idle");
  Serial.print(" count=");
  Serial.println(motionCount);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PIR on D7");
  lcd.setCursor(0, 1);
  lcd.print("Warming up...");

  Serial.println("PIR + LCD monitor");

  for (int secondsLeft = 30; secondsLeft > 0; secondsLeft--) {
    lcd.setCursor(12, 1);
    if (secondsLeft < 10) {
      lcd.print(" ");
    }
    lcd.print(secondsLeft);
    lcd.print("s");
    delay(1000);
  }

  lcd.clear();
  showStatus(digitalRead(PIR_PIN) == HIGH);
}

void loop() {
  bool motion = digitalRead(PIR_PIN) == HIGH;

  if (motion && !lastMotion) {
    motionCount++;
  }

  if (motion != lastMotion) {
    showStatus(motion);
    lastMotion = motion;
  }

  delay(100);
}
