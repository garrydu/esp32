
/*
Author G Du, Sep 2025

A few learnings:
  * Xiao C3 I2C also uses 4 to SDA and 5 to SCL
  * The A4988 chip has the enable LOW as run, and HIGH as stop.
  * The Wowki simulator doesn't have to connect the element vcc and gnd wires, they powered by default.
  * The display I2C will block others when the pulse is sent in the same routine.

Using FreeRTOS set 3 seperate threads
  1. monitoring the on/off switch, priority=1
  2. sending pulse and cnt position, priority=2
  3. update speed setting pins and refresh display priority=1

the priority should not be set to zero, when set to 0, it becomes very slow. setting to 3 also
doesn't make it faster.

reading digital pin input is slower than reading a variable, obviously. Don't know if it is
caused by the simulator, putting the reading pin function directly into the pulse thread will
make the speed inconsistent.

*/

#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

TaskHandle_t lcdTaskHandle = NULL;
TaskHandle_t pwmTaskHandle = NULL;
TaskHandle_t switchTaskHandle = NULL;
/* The task handle here is the pointer to use for kill or pause the thread from
other part of the code */

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

#define DIR_PIN D7
#define STEP_PIN D8
#define UNIT_STEP 0.1125
#define MS1 D0 // MUST use D0 instead of 0, the pin D0 is not 0 in integer.
#define MS2 D10
#define MS3 D9

int step_MS_code[5] = {0, 4, 2, 6, 7};
long long step_size[5] = {16, 8, 4, 2, 1};
long long unit_step_cnt = 0;
int current_step_size = 0;
bool motor_enabled = false;

// #define ENABLE_PIN D2
#define SWITCH_PIN D6
#define ADC_INPUT_PIN A1

void print_speed(int idx)
{

  float spd = 0;

  char str[100];

  if (motor_enabled)
  {
    // spd_lvl=5-idx;
    spd = step_size[idx] * 0.1125;
  }

  lcd.setCursor(0, 0);
  sprintf(str, "%01.4f deg/step", spd);
  lcd.print(str);

  long long display_s = unit_step_cnt % (3200LL);
  float degree = (float)(display_s) * (float)(UNIT_STEP);

  lcd.setCursor(0, 1);
  sprintf(str, "pos: %07.3f deg", degree);
  lcd.print(str);
}

void set_motor_by_idx(int step_idx)
{

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
  current_step_size = step_size[step_idx];
}

void lcdUpdateTask(void *parameter)
{
  int idx;

  for (;;)
  {
    int sensorValue = analogRead(ADC_INPUT_PIN); // Read analog input on pin A1

    idx = 4 - sensorValue / 1000;
    if (idx > 4)
      idx = 4;
    if (idx < 0)
      idx = 0;

    set_motor_by_idx(idx);

    print_speed(idx);

    vTaskDelay(100 / portTICK_PERIOD_MS);
    // Not update every 100ms, but wait 100ms between the
    // 2 updates, the real update rate will depend on the chip
  }
}

void pwmTask(void *parameter)
{

  while (1)
  {

    if (motor_enabled)
    {
      digitalWrite(STEP_PIN, HIGH);
      unit_step_cnt += current_step_size;
      digitalWrite(STEP_PIN, LOW);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // wait one tick
    /*
    On the Seeed Studio XIAO ESP32-C3, the default FreeRTOS system tick rate is 100 Hz,
    which means one tick occurs every 10 milliseconds (10 ms per tick). This setting is
    common for ESP32-C3 chips in both ESP-IDF and Arduino environments unless explicitly
    changed in the firmware build configuration.
      Tick rate (configTICK_RATE_HZ): 100 Hz (default)
      Tick period (portTICK_PERIOD_MS): 10 ms (default)
    */
  }
}

void switchTask(void *parameter)
{

  while (1)
  {

    if (digitalRead(SWITCH_PIN) == LOW)
    {
      motor_enabled = true;
    }
    else
    {
      motor_enabled = false;
    }

    vTaskDelay(40 / portTICK_PERIOD_MS); // wait 4 ticks
  }
}

void setup()
{

  Serial.begin(115200);

  pinMode(SWITCH_PIN, INPUT);
  pinMode(STEP_PIN, OUTPUT);
  // pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
  digitalWrite(DIR_PIN, HIGH);
  // digitalWrite(ENABLE_PIN, LOW);
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

  xTaskCreatePinnedToCore(
      pwmTask,        // Task function
      "PWM Task",     // Name of task
      4096,           // Stack size in bytes
      NULL,           // Task input parameter
      2,              // Priority
      &pwmTaskHandle, // Task handle
      0               // Run on core 0
  );

  xTaskCreatePinnedToCore(
      switchTask,        // Task function
      "Switch Task",     // Name of task
      4096,              // Stack size in bytes
      NULL,              // Task input parameter
      1,                 // Priority
      &switchTaskHandle, // Task handle
      0                  // Run on core 0
  );
}

void loop(void)
{
  // nothing
}
