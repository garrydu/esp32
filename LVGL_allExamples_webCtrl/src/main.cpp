#include <Arduino.h>
#include "example_funcs.h"
// #include "display.h"
#include "connect_wifi.h"
// #include "page.h"
#include "webpage.h"
/*
   #include "../.pio/libdeps/seeed_xiao_esp32s3/lvgl/examples/lv_examples.h"
   Here we should include the lv_examples.h file from the lvgl library. However,
   to make it neater, the include is done in the platformio.ini file using
   build_flags
   */

void setup()
{
    Serial.begin(115200);

    // Display::display_init();
    // Page::new_page(func_list[0]);
    // delay(1000); // Wait for display init
    // printf("Display Task init done.\n");

    WIFI::connectToWiFi();
    Serial.println("WiFi connected.");
    WebPage::init_html_pages();
    Serial.println("Done Update page.");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Check every 100 ms
}

void loop()
{
    // Display::display_handler();
    vTaskDelay(2 / portTICK_PERIOD_MS); // Check every 100 ms
    WebPage::server.handleClient();
    vTaskDelay(2 / portTICK_PERIOD_MS); // Check every 100 ms
}