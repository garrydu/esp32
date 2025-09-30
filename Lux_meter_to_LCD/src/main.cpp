#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <stdio.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2
#define analog_In D2
// #define I2C_SDA 4
// #define I2C_SCL 5

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

float read_lux()
{
  const float GAMMA = 0.7;
  const float RL10 = 50;

  // Analog: Convert the analog value into lux value:
  int analogValue = analogRead(analog_In);
  float voltage = analogValue / 4096. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux_a = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  return lux_a;
}

void setup()
{
  Serial.begin(115200);
  pinMode(analog_In, INPUT);

  Serial.println("");
  Serial.println("Welcome to Wokwi :-)");

  Wire.begin(); // I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.print("....Boot OK.....");
}

void loop()
{
  float lux = read_lux();
  char lux_str[200];

  // Print float to string with 2 decimal places
  sprintf(lux_str, "%.2f", lux);

  Serial.print("Lux reading: ");
  Serial.println(lux_str);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lux_str);
  lcd.println(" lx");

  delay(1000);
}
