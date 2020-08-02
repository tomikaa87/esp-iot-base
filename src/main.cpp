#include "Config.h"
#include "Application.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <FS.h>

static std::unique_ptr<Application> application;

void ICACHE_RAM_ATTR timer1Isr()
{
    if (application) {
        application->epochTimerIsr();
    }
}

void initializeEpochTimer()
{
    timer1_isr_init();
    timer1_attachInterrupt(timer1Isr);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
    timer1_write(100);
}

void initializeSerial()
{
    Serial.begin(74880);
}

void initWiFi()
{
    Serial.println("Connecting to WiFi");
    WiFi.begin(Config::WiFi::SSID, Config::WiFi::Password);
}

#include "drivers/SimpleI2C.h"
#include "drivers/MCP7940N.h"

using rtc = Drivers::MCP7940N;

void setup()
{
    initializeEpochTimer();
    initializeSerial();
    initWiFi();

    application.reset(new Application);

    Serial.println("Initialization finished");
}

void loop()
{
    if (application)
        application->task();
}