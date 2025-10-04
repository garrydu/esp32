#include <Arduino.h>
#include "connect_wifi.h"
#include "webpage.h"

// Define pins for LEDs and buttons
const uint8_t pins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
const int num_pins = sizeof(pins) / sizeof(pins[0]);

void data2XML()
{
    int button_states[num_pins];
    // if (WebPage::webSubmittedValue >= 0)
    // {
    for (int i = 0; i < num_pins; i++)
        button_states[i] = digitalRead(pins[num_pins - 1 - i]) == LOW;
    WebPage::update_XML(button_states, num_pins);
    // WebPage::webSubmittedValue = -1;
    // }
}

void setup()
{
    Serial.begin(115200);

    for (int i = 0; i < num_pins; i++)
        pinMode(pins[i], INPUT_PULLUP);

    WIFI::connectToWiFi();
    Serial.println("WiFi connected.");
    WebPage::init_html_pages(data2XML); 
    /* 
    This function transfer enable the Webpage routines update
    the MCU data to XML string on demand. The previous projects use
    status variable to trigger the updates which is not timely enough,
    and hard to work with dual interface of webpage and API. 
    */
    Serial.println("Done Update page.");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Check every 100 ms
}

void loop()
{
    vTaskDelay(2 / portTICK_PERIOD_MS); // Check every 100 ms
    WebPage::server.handleClient();
    vTaskDelay(2 / portTICK_PERIOD_MS); // Check every 100 ms
}