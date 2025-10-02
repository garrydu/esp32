#include <Arduino.h>
#include "example_funcs.h"
#include "display.h"
#include "connect_wifi.h"
// #include "lvgl.h"
// #include "../.pio/libdeps/seeed_xiao_esp32s3/lvgl/examples/lv_examples.h"

// The parameters for handling example functions
const int func_list_size = func_list.size();
int current_func = 0;
// bool change_page = false;
TaskHandle_t updateTaskHandle = NULL;

void update_page_task(void *pvParameters)
{
    Serial.print("Switching to function: ");
    Serial.println(func_names[current_func]);
    vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay to allow screen to clear
    func_list[current_func]();
    change_page = false;
    Serial.println("Function completed.");
    // After the function is done, delete this task itself
    updateTaskHandle = NULL;
    vTaskDelete(NULL); // Delete this task
    Serial.println("Update Display Task deleted.");
}

void new_page_thread()
{
    xTaskCreatePinnedToCore(
        update_page_task,    // Task function
        "UpdateDisplayTask", // Name of the task (for debugging)
        4096,                // Stack size (bytes)
        NULL,                // Parameter to pass to the task
        2,                   // Task priority
        &updateTaskHandle,   // Task handle
        1                    // Run on core 1
    );
}

void change_page()
{
    Serial.println("Change page triggered");
    Serial.print("New function index: ");
    Serial.println(current_func);

    // kill the previous page updating thread if it exists
    if (updateTaskHandle != NULL)
    {
        Serial.println("The display handle existing. Deleting previous display task");
        vTaskDelete(updateTaskHandle);
        updateTaskHandle = NULL;
        vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay to ensure task is deleted
        Serial.println("Previous display task deleted");
    }

    Serial.println("Stopping current display task");

    new_display_page();

    Serial.println("Cleaned current screen");
    Serial.println("Starting new display task");

    new_page_thread();

    Serial.println("Done starting new display task");
    vTaskDelay(500 / portTICK_PERIOD_MS); // Check every 100 ms
}

void setup()
{
    Serial.begin(115200);
    connectToWiFi();
    display_init();
    // change_page = true;
    new_page_thread();
    Serial.println("Done Update Display Task init.");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Check every 100 ms
}

void loop()
{

    vTaskDelay(2 / portTICK_PERIOD_MS); // Check every 100 ms
    change_page();
}