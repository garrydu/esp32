#include <Arduino.h>
#include "lvgl.h"
#include <TFT_eSPI.h>
#include <Adafruit_FT6206.h>
#include <LiquidCrystal_I2C.h>
#include "example_funcs.h"
// #include "../lib/lvgl-8.3.11/examples/lv_examples.h"

// The parameters for handling example functions
const int func_list_size = func_list.size();
int current_func = 0;
int next_button_state = HIGH;
int prev_button_state = HIGH;
int go_button_state = HIGH;
bool change_page = false;
TaskHandle_t updateTaskHandle = NULL;

// Define the two push button pins
#define BUTTON_PIN_NEXT D3
#define BUTTON_PIN_PREV D6
#define BUTTON_PIN_GO D7

// Set the LCD I2C address, number of columns and rows
#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 touch = Adafruit_FT6206();
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

// Simple function to print text with auto-wrap (max 2 lines, starts at 0,0)
void printAutoWrap(const char *text)
{
    lcd.clear();
    lcd.setCursor(0, 0); // Start at position 0,0

    int textLength = strlen(text);
    int maxChars = LCD_COLUMNS * LCD_LINES; // Maximum characters that fit (32 for 16x2)

    // Limit to what fits on 2 lines
    if (textLength > maxChars)
    {
        textLength = maxChars;
    }

    for (int i = 0; i < textLength; i++)
    {
        // When we reach the end of first line, move to second line
        if (i == LCD_COLUMNS)
        {
            lcd.setCursor(0, 1); // Move to start of second line
        }

        lcd.print(text[i]);
    }
}

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp_drv);
}

void my_touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    bool touched = touch.touched();
    if (touched)
    {
        TS_Point point = touch.getPoint();

        // Print raw touch coordinates
        Serial.print("Raw X: ");
        Serial.print(point.x);
        Serial.print(", Y: ");
        Serial.print(point.y);

        // Map the touch coordinates to match the screen resolution and orientation

        uint16_t touchX = point.x;
        uint16_t touchY = point.y;

        // touchX = map(point.x, 0, 320, 320, 0);
        touchY = map(point.y, 0, 320, 320, 0);

        data->point.x = touchY;
        data->point.y = touchX;

        Serial.print("\t\tTouched at X: ");
        Serial.print(touchX);
        Serial.print(", Y: ");
        Serial.println(touchY);
    }

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
    }
}

void update_display_task(void *pvParameters)
{
    if (change_page)
    {
        printAutoWrap(func_names[current_func]);
        Serial.print("Switching to function: ");
        Serial.println(func_names[current_func]);
        vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay to allow screen to clear
        func_list[current_func]();
        change_page = false;
        Serial.println("Function completed.");
        updateTaskHandle = NULL;
        vTaskDelete(NULL); // Delete this task
        Serial.println("Update Display Task deleted.");
    }
}

void check_button_states()
{
    if (!change_page)
    {
        int next_button_read = digitalRead(BUTTON_PIN_NEXT);
        int prev_button_read = digitalRead(BUTTON_PIN_PREV);
        int go_button_read = digitalRead(BUTTON_PIN_GO);
        // Check for button state changes
        if (next_button_read != next_button_state)
        {
            next_button_state = next_button_read;
            if (next_button_state == LOW)
            {
                Serial.println("Next button pressed");
                // change_page = true;
                current_func++;
                if (current_func >= func_list_size)
                    current_func = 0;
                printAutoWrap(func_names[current_func]);
            }
        }
        else if (prev_button_read != prev_button_state)
        {
            prev_button_state = prev_button_read;
            if (prev_button_state == LOW)
            {
                Serial.println("Previous button pressed");
                // change_page = true;
                current_func--;
                if (current_func < 0)
                    current_func = func_list_size - 1;
                printAutoWrap(func_names[current_func]);
            }
        }
        else if (go_button_read != go_button_state)
        {
            go_button_state = go_button_read;
            if (go_button_state == LOW)
            {
                Serial.println("Go button pressed");
                change_page = true;
                Serial.println("Change page set to true");
                Serial.print("Current function index: ");
                Serial.println(current_func);
            }
        }
    }
}

void button_check_task(void *pvParameters)
{
    while (1)
    {
        check_button_states();
        vTaskDelay(200 / portTICK_PERIOD_MS); // Check every 100 ms
    }
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */
    lv_init();
    tft.begin();
    tft.setRotation(3);

    if (!touch.begin(40))
    { // pass in 'sensitivity' coefficient
        Serial.println("Couldn't start FT6206 touchscreen controller");
    }
    else
    {
        Serial.println("FT6206 touchscreen controller CONNECTED!");
    }

    lv_disp_draw_buf_init(&disp_buf, buf, NULL, screenWidth * 10);

    static lv_disp_drv_t disp_drv;     /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);       /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf;     /*Set an initialized buffer*/
    disp_drv.flush_cb = my_disp_flush; /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = screenWidth;    /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = screenHeight;   /*Set the vertical resolution in pixels*/
    lv_disp_t *disp;
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

    // lv_disp_drv_register(&disp_drv);
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*See below.*/
    indev_drv.read_cb = my_touchpad_read;   /*See below.*/
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);

    // Check vertical and horizontal resolution of what lvgl sees
    lv_coord_t hor_res = lv_disp_get_hor_res(NULL);
    Serial.print("LVGL Horizontal Resolution: ");
    Serial.println(hor_res);

    // Check vertical and horizontal resolution of what lvgl sees
    lv_coord_t ver_res = lv_disp_get_ver_res(NULL);
    Serial.print("LVGL Vertical Resolution: ");
    Serial.println(ver_res);

    lv_coord_t disp_dpi = lv_disp_get_dpi(NULL);
    Serial.print("LVGL Display DPI: ");
    Serial.println(disp_dpi);

    Wire.begin(); // Use default pins
    lcd.init();
    lcd.backlight();
    lcd.print("Initiated.");
    Serial.println("Done LCD init.");

    // Initialize the two push buttons
    pinMode(BUTTON_PIN_NEXT, INPUT_PULLUP);
    pinMode(BUTTON_PIN_PREV, INPUT_PULLUP);
    pinMode(BUTTON_PIN_GO, INPUT_PULLUP);
    Serial.println("Done Button init.");

    // Create a task for checking button states
    // xTaskCreatePinnedToCore(
    //     button_check_task, // Task function
    //     "ButtonCheckTask", // Name of the task (for debugging)
    //     2048,              // Stack size (bytes)
    //     NULL,              // Parameter to pass to the task
    //     1,                 // Task priority
    //     NULL,              // Task handle
    //     0                  // Run on core 0
    // );
    // Serial.println("Done Button Check Task init.");

    change_page = true;
    xTaskCreatePinnedToCore(
        update_display_task, // Task function
        "UpdateDisplayTask", // Name of the task (for debugging)
        4096,                // Stack size (bytes)
        NULL,                // Parameter to pass to the task
        20,                  // Task priority
        &updateTaskHandle,   // Task handle
        1                    // Run on core 1
    );
    Serial.println("Done Update Display Task init.");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Check every 100 ms
}

void loop()
{
    // lv_tick_inc(5);
    /* tell LVGL how much time has passed. However if LV_TICK_CUSTOM is set to 1
     * this call is not necessary. For Aduirno system it will read the system time
     itself.*/
    lv_task_handler();
    check_button_states();
    vTaskDelay(2 / portTICK_PERIOD_MS); // Check every 100 ms
    if (change_page)
    {
        Serial.println("Change page triggered");
        Serial.print("Current function index: ");
        Serial.println(current_func);
        Serial.println("Stopping current display task");
        // lv_obj_clean(lv_scr_act());
        lv_obj_t *old_screen = lv_scr_act();
        lv_obj_t *new_screen = lv_obj_create(NULL);
        // Populate new_screen as desired
        lv_scr_load(new_screen);
        lv_obj_del(old_screen); // Safe deletion

        Serial.println("Cleaned current screen");
        Serial.println("Starting new display task");
        if (updateTaskHandle != NULL)
        {
            Serial.println("The display handle existing. Deleting previous display task");
            vTaskDelete(updateTaskHandle);
            updateTaskHandle = NULL;
            vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay to ensure task is deleted
            Serial.println("Previous display task deleted");
        }
        xTaskCreatePinnedToCore(
            update_display_task, // Task function
            "UpdateDisplayTask", // Name of the task (for debugging)
            4096,                // Stack size (bytes)
            NULL,                // Parameter to pass to the task
            2,                   // Task priority
            &updateTaskHandle,   // Task handle
            // NULL,
            1 // Run on core 1
        );
        Serial.println("Done starting new display task");
        vTaskDelay(500 / portTICK_PERIOD_MS); // Check every 100 ms
    }
}
