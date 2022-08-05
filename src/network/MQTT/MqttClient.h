#pragma once

#include "Logger.h"

#include <PubSubClient.h>
#include <WiFiClient.h>

#include <queue>
#include <vector>

#include <pgmspace.h>

class ApplicationConfig;
class MqttVariableBase;

class MqttClient
{
public:
    MqttClient(const ApplicationConfig& appConfig);

    void task();

    bool publish(PGM_P topic, const std::string& payload);

    void subscribe(PGM_P topic, MqttVariableBase* base);
    void unsubscribe(PGM_P topic, MqttVariableBase* base);

private:
    const ApplicationConfig& _appConfig;
    Logger _log{ "MqttClient" };
    WiFiClient _wifiClient;
    PubSubClient _client;
    unsigned long _lastConnectAttemptTime = 0;
    
    struct Variable {
        Variable(PGM_P topic, MqttVariableBase* base)
            : topic(topic)
            , base(base)
        {}

        PGM_P topic = nullptr;
        MqttVariableBase* base = nullptr;
    };

    std::vector<Variable> _variables;
    std::vector<std::string> _pendingUnSubscriptions;
    std::queue<std::pair<std::string, std::string>> _pendingPublishes;

    void onClientCallback(const char* topic, const uint8_t* payload, const unsigned int length);
};