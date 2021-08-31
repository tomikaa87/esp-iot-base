#pragma once

#include "Logger.h"

#include <PubSubClient.h>
#include <WiFiClient.h>

#include <vector>

class ApplicationConfig;
class MqttVariableBase;

class MqttClient
{
public:
    MqttClient(const ApplicationConfig& appConfig);

    void task();

    void publish(const char* topic, const std::string& payload);

    void subscribe(const char* topic, MqttVariableBase* base);
    void unsubscribe(const char* topic, MqttVariableBase* base);

private:
    const ApplicationConfig& _appConfig;
    Logger _log{ "MqttClient" };
    WiFiClient _wifiClient;
    PubSubClient _client;
    unsigned long _lastConnectAttemptTime = 0;
    unsigned long _lastUpdateTime = 0;
    
    struct Variable {
        Variable(const char* topic, MqttVariableBase* base)
            : topic(topic)
            , base(base)
        {}

        const char* topic = nullptr;
        MqttVariableBase* base = nullptr;
    };

    std::vector<Variable> _variables;

    void onClientCallback(const char* topic, const uint8_t* payload, const unsigned int length);
};