#include "CoreApplication.h"

#include <Arduino.h>

static CoreApplication* application = nullptr;

void setup()
{
    static ApplicationConfig appConfig;

    static CoreApplication app(appConfig);
    application = &app;

    Serial.println("Initialization finished");
}

void loop()
{
    if (application)
        application->task();
}