
/*
Author G Du, Sep 2025

A few learnings:
Xiao C3 I2C also uses 4 to SDA and 5 to SCL
The A4988 chip has the enable LOW as run, and HIGH as stop.
The Wowki simulator doesn't have to connect the element vcc and gnd wires, they powered by default.
Using PWM pin to send the step pulses is more stable than using an interrupt func,
maybe try interrupt again
The display I2C will block others when the pulse is sent in the same routine.
Move the display routine to a FreeRTOS task
The PWM setting low freq requires high bit
*/

#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

TaskHandle_t lcdTaskHandle = NULL;

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define DIR_PIN D7
// #define STEP_PIN 8
#define UNIT_STEP 0.1125
#define MS1 D0 // MUST use D0 instead of 0, the pin D0 is not 0 in integer.
#define MS2 D10
#define MS3 D9

int step_MS_code[5] = {0, 4, 2, 6, 7};
long long step_size[5] = {16, 8, 4, 2, 1};

const int ledPin = D8;    // GPIO pin
const int pwmChannel = 0; // PWM channel (0-5)
const int pwmFrequency = 200;
const int pwmResolution = 13; // bits

#define INPUT_PIN D2

void print_speed(int idx)
{
  int spd_lvl = 0;
  float spd = 0;

  char str[100];

  if (idx < 0)
  {
    spd_lvl = 0;
    spd = 0;
  }
  else
  {
    spd_lvl = 5 - idx;
    spd = step_size[idx] * 0.1125;
  }

  lcd.setCursor(0, 0);
  sprintf(str, "speed lvl: %2d", spd_lvl);
  lcd.print(str);
  lcd.setCursor(0, 1);
  sprintf(str, "%.4f deg/stp", spd);
  lcd.print(str);
}

void set_motor_by_idx(int step_idx)
{
  // Serial.print("  idx ");
  // Serial.print(step_idx);

  int step_idx_tmp = step_MS_code[step_idx];
  // Serial.print(" MS setting: ");
  digitalWrite(MS3, step_idx_tmp % 2);
  // Serial.print(step_idx_tmp%2);
  step_idx_tmp >>= 1;
  digitalWrite(MS2, step_idx_tmp % 2);
  // Serial.print(step_idx_tmp%2);
  step_idx_tmp >>= 1;
  digitalWrite(MS1, step_idx_tmp % 2);
  // Serial.print(step_idx_tmp%2);

  // current_step_size =step_size[step_idx];

  print_speed(step_idx);
}

void lcdUpdateTask(void *parameter)
{
  int counter = 0, idx;
  for (;;)
  {
    int sensorValue = analogRead(A1); // Read analog input on pin A0

    int pinState = digitalRead(INPUT_PIN); // Read the pin state HIGH or LOW

    if (pinState == HIGH)
    {
      idx = -1;
    }
    else
    {
      idx = 4 - sensorValue / 1000;
      if (idx > 4)
        idx = 4;
      if (idx < 0)
        idx = 0;
    }

    set_motor_by_idx(idx);

    vTaskDelay(50 / portTICK_PERIOD_MS); // Update every 1/20 second
  }
}

void setup()
{

  Serial.begin(115200);

  pinMode(INPUT_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  // digitalWrite(STEP_PIN, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
  Serial.println("Done motor init.");

  Wire.begin(); // So far can only use 4 and 5 for I2C
  lcd.init();
  lcd.backlight();
  Serial.println("Done LCD init.");

  xTaskCreatePinnedToCore(
      lcdUpdateTask,     // Task function
      "LCD Update Task", // Name of task
      4096,              // Stack size in bytes
      NULL,              // Task input parameter
      1,                 // Priority
      &lcdTaskHandle,    // Task handle
      0                  // Run on core 0
  );

  // Attach channel, frequency, resolution to pin
  // bool success = ledcAttachChannel(ledPin, pwmFrequency, pwmResolution, pwmChannel);

  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  // bool success =
  ledcAttachPin(ledPin, pwmChannel);

  // if (!success)
  // {
  //   Serial.println("PWM attach failed");
  // }
  ledcWrite(pwmChannel, 10);

  // lcd.clear();
  // lcd.println("Init Done.");
  Serial.println("Done PWM init.");

  delay(200);
}

void loop(void)
{
  // nothing
}
