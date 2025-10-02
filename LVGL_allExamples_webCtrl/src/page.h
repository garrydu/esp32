#ifndef PAGE_H
#define PAGE_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Page {

// extern TaskHandle_t updateTaskHandle;

void kill_thread_if_exists();
void update_page_task(void *pvParameters);
void new_page_thread(void (*page_func)() = nullptr);
void new_page(void (*page_func)() = nullptr);

} // namespace Page

#endif // PAGE_H
