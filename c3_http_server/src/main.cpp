#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "webpage.h"
#include "connect_wifi.h"

// Set the LCD I2C address, columns and rows, and initialize the display
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Define pins for button and joystick
#define BUTTON_PIN D2
#define JOYSTICK_X_PIN D1
#define JOYSTICK_Y_PIN D0
#define JOYSTICK_BUTTON_PIN D3

// Variables to hold the state of the button and joystick
int buttonState;
int joystickX;
int joystickY;
int joystickButtonState;

int currentlyDisplayedValue = -1;

void setup(void)
{
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.print("LCD init done. ");
  Serial.println("Pin and LCD Setup done.");

  if (!connectToWiFi())
  {
    Serial.println("WiFi Failed!");
    while (true)
      ;
  }

  WebPage::init_html_pages();
  lcd.setCursor(0, 2);
  lcd.print("Server started.   ");
  delay(1000);
  lcd.clear();
}

int status_value()
{
  int newButtonState = digitalRead(BUTTON_PIN);
  int newJoystickX = analogRead(JOYSTICK_X_PIN);
  int newJoystickY = analogRead(JOYSTICK_Y_PIN);
  int newJoystickButtonState = digitalRead(JOYSTICK_BUTTON_PIN);

  lcd.setCursor(0, 1);
  lcd.printf("Btn:%s    ", newButtonState == HIGH ? "UP" : "DN");
  lcd.setCursor(0, 2);
  lcd.printf("X:%4d Y:%4d ", newJoystickX, newJoystickY);
  lcd.setCursor(0, 3);
  lcd.printf("JS Btn:%s ", newJoystickButtonState == HIGH ? "UP" : "DN");

  int val = 0;
  // only response to keys when released after being pressed
  // this will avoid multiple key presses when holding down the button, or triggered by short delay time in loop()
  if (buttonState == LOW and newButtonState == HIGH or joystickButtonState == LOW and newJoystickButtonState == HIGH)
  {
    val = 1;
  }
  if (joystickX < 2048 and newJoystickX == 2048)
    val = val | 2;
  if (joystickX > 2048 and newJoystickX == 2048)
    val = val | 4;
  if (joystickY < 2048 and newJoystickY == 2048)
    val = val | 8;
  if (joystickY > 2048 and newJoystickY == 2048)
    val = val | 16;

  buttonState = newButtonState;
  joystickX = newJoystickX;
  joystickY = newJoystickY;
  joystickButtonState = newJoystickButtonState;

  return val;
}

void loop(void)
{
  WebPage::server.handleClient();

  int status_value_now = status_value();
  if (status_value_now > 0)
    WebPage::newKeyInput(status_value_now);

  // Update LCD if a new web value has been submitted
  if (WebPage::webSubmittedValue != -1 && WebPage::webSubmittedValue != currentlyDisplayedValue)
  {
    currentlyDisplayedValue = WebPage::webSubmittedValue;
    lcd.setCursor(0, 0);
    lcd.printf("Received: %d   ", currentlyDisplayedValue);
    WebPage::webSubmittedValue = -1; // Reset after displaying
  }
  delay(2);
}
