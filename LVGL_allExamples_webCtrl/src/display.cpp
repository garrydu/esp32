#include <Arduino.h>
#include <TFT_eSPI.h>
#include "lvgl.h"
#include <Adafruit_FT6206.h>
#include "display.h"

namespace Display
{

    static const uint16_t screenWidth = 320;
    static const uint16_t screenHeight = 240;

    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buf[screenWidth * screenHeight / 10];

    // The FT6206 uses hardware I2C (SCL/SDA)
    Adafruit_FT6206 touch = Adafruit_FT6206();
    TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

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

    void display_init()
    {
        Serial.println("Starting display initialization...");
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

        Serial.println("Display initialization complete.");
    }

    void new_display_page()
    {
        lv_obj_t *old_screen = lv_scr_act();
        lv_obj_t *new_screen = lv_obj_create(NULL);
        // Populate new_screen as desired
        lv_scr_load(new_screen);
        if (old_screen && old_screen != new_screen)
        {
            lv_obj_del(old_screen); // Safe deletion
        }
        Serial.println("Screen changed");
    }

    void display_handler()
    {
        lv_task_handler();
    }

} // namespace Display