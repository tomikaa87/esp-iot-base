# esp-iot-base
This code is supposed to be a minimal base for an ESP8266-based IoT application using Platform.io

# Features

* Easy-to-use MQTT client
* Over-the-Air (OTA) updates via:
  * ArduinoOTA (also with Flash/LittleFS support)
  * HTTP
* Real Time Clock (RTC) integration:
  * Hardware-based (MCP7940N)
  * Software-based (ESP8266's internal timer)
* Category-based logging with Syslog integration for remote logging
* Persistent storage with:
  * Internal flash (EEPROM emulation)
  * External EERAM (Microchip 47L(04/16)
* More features to come later...

# Examples

There are no specific examples (yet), but see my other projects based on this library:
* [esp-thermostat](https://github.com/tomikaa87/esp-thermostat)
* [esp-irrigation-controller](https://github.com/tomikaa87/esp-irrigation-controller)
* [esp-smc (Shutter Motor Controller)](https://github.com/tomikaa87/esp-smc)
* [esp-smch (Shutter Motor Controller Hub)](https://github.com/tomikaa87/esp-smch)
* [esp-door-phone-monitor](https://github.com/tomikaa87/esp-door-phone-monitor)
