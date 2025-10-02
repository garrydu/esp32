#include "page.h"
#include "display.h"

namespace Page
{

    TaskHandle_t updateTaskHandle = NULL;

    void kill_thread_if_exists()
    {
        if (updateTaskHandle != NULL)
        {
            Serial.println("Killing existing update task...");
            vTaskDelete(updateTaskHandle);
            updateTaskHandle = NULL;
            Serial.println("Existing update task killed.");
        }
    }

    void update_page_task(void *pvParameters)
    {
        auto page_func = reinterpret_cast<void (*)()>(pvParameters);
        if (page_func == nullptr)
        {
            Serial.println("No page function provided. Skipping update.");
            updateTaskHandle = NULL;
            vTaskDelete(NULL);
            return;
        }
        Serial.println("Switching to new page function...");
        vTaskDelay(100 / portTICK_PERIOD_MS);
        page_func();
        Serial.println("Function completed.");
        updateTaskHandle = NULL;
        vTaskDelete(NULL);
    }
/*
Default arguments for function parameters should only be specified in the header 
file, not in both the header and the implementation. Remove = nullptr from the
 function definitions in your .cpp file, and keep it only in the .h file.
*/
    void new_page_thread(void (*page_func)() /* = nullptr */)
    {
        xTaskCreatePinnedToCore(
            update_page_task,                    // Task function
            "UpdateDisplayTask",                 // Name of the task (for debugging)
            4096,                                // Stack size (bytes)
            reinterpret_cast<void *>(page_func), // Parameter to pass to the task
            2,                                   // Task priority
            &updateTaskHandle,                   // Task handle
            1                                    // Run on core 1
        );
    }

    void new_page(void (*page_func)() )
    {
        kill_thread_if_exists();

        Display::new_display_page();

        Serial.println("Cleaned current screen");
        Serial.println("Starting new display task");

        new_page_thread(page_func);

        Serial.println("Done starting new display task");
        vTaskDelay(500 / portTICK_PERIOD_MS); // Check every 100 ms
    }
}