#include <WiFi.h>
#include <Arduino.h>
#include "connect_wifi.h"

bool connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = WIFI_TIMEOUT;  // timeout duration in milliseconds (e.g., 15 seconds)

  // Wait for connection with timeout
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime >= timeout) {
      Serial.println("\nWiFi connection timed out.");
      return false; // Timeout, connection failed
    }
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nConnected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  return true;  // Connected successfully
}

void disconnectFromWiFi() {
  WiFi.disconnect();
  Serial.println("Disconnected from WiFi");
}