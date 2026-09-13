#include <LiquidCrystal.h>

const int LCD_RS = 12;
const int LCD_ENABLE = 11;
const int LCD_D4 = 5;
const int LCD_D5 = 4;
const int LCD_D6 = 3;
const int LCD_D7 = 2;

LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello Arduino");
  lcd.setCursor(0, 1);
  lcd.print("LCD is online");

  Serial.println("LCD display test is running");
}

void loop() {
  static unsigned long lastUpdate = 0;
  static unsigned int seconds = 0;

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    seconds++;

    lcd.setCursor(0, 1);
    lcd.print("Seconds: ");
    lcd.print(seconds);
    lcd.print("     ");

    Serial.print("seconds=");
    Serial.println(seconds);
  }
}
