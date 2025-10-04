# Custom scanf for ESP32 Serial

A scanf working with the Serial interface. The input will wait for user input and wrapped with the native scanf function with format string support. While printf works by default with the Serial interface, Serial.print is not necessary for Arduino platform.