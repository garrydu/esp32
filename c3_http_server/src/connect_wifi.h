#pragma once

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6
#define WIFI_TIMEOUT 15000  // 15 seconds timeout for WiFi connection

bool connectToWiFi();
void disconnectFromWiFi();