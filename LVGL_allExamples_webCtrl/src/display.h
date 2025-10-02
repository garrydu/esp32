#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "lvgl.h"
#include <Adafruit_FT6206.h>

namespace Display
{

    void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
    void my_touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
    void display_init();
    void new_display_page();
    void display_handler();

} // namespace Display

#endif // DISPLAY_H
