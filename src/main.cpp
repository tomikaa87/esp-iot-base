#include "CoreApplication.h"

#include <Arduino.h>

static std::unique_ptr<CoreApplication> application;

void setup()
{
    static ApplicationConfig appConfig;

    application.reset(new CoreApplication(appConfig));

    Serial.println("Initialization finished");
}

void loop()
{
    if (application)
        application->task();
}