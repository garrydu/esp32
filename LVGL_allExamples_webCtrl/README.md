# LVGL all example demo controlled by webpage

This project uses webpage to select the LVGL example demo and display on a ILI9341 touch screen.

## The Wokwi simulator does not work with WIFI and LVGL together, need test this on real MCU later.
So far the code work with WIFI and WebServer, and the webpage sends back user selection without any problems. 

The display and page update have been put into seperated modules. The page updating function just need a page init function as parameter to start, will try to use it do more in the future. 

## Notes
### firmware.elf ld dram overflow
After adding Wifi, it gives the error during compiling. The effective way is to reduce the buffer size in LVGL in lv_conf.h
```
#define LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE (8 * 1024)
#define LV_MEM_SIZE (16 * 1024)  // or smaller if fits RAM
```
### Custom tick
AI suggeted to remove the custom tick, however it doesn't work with wokwi, or Arduino. Have to keep it unchanged.
```
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis()) 
```
### Pre build script
PlatformIO allows to run scripts automatically before each build. For example I used it to convert HTML file to header file, and copy the lv_conf.h to the pio lib folder. This avoids manual operation and made the code good for migration. To set the script use:
```
extra_scripts = pre:pre_build_scripts.py
```
### Send HTML in WebServer
Have to use (const char *) even have string.to_cstr()
