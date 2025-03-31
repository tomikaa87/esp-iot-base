#pragma once

#include "Logger.h"

#include <PubSubClient.h>
#include <WiFiClient.h>

#include <functional>
#include <queue>
#include <sstream>
#include <vector>

#include <pgmspace.h>

class ApplicationConfig;
class MqttVariableBase;

class MqttClient
{
public:
    explicit MqttClient(const ApplicationConfig& appConfig);

    void task();

    bool publish(PGM_P topic, const std::string& payload, bool dropWhenNotConnected = true);
    bool publish(const std::string& topic, const std::string& payload, bool dropWhenNotConnected = true);

    using StringGenerator = std::function<void(std::stringstream&)>;

    bool publish(StringGenerator&& topic, StringGenerator&& payload);

    // void subscribe(PGM_P topic, MqttVariableBase* base);
    void registerVariable(MqttVariableBase* base);
    // void unsubscribe(PGM_P topic, MqttVariableBase* base);
    void unregisterVariable(MqttVariableBase* base);

    [[nodiscard]] bool isConnected() const;

private:
    const ApplicationConfig& _appConfig;
    Logger _log{ "MqttClient" };
    WiFiClient _wifiClient;
    PubSubClient _client;
    unsigned long _lastConnectAttemptTime = 0;

    std::vector<MqttVariableBase*> _variables;
    std::vector<std::string> _pendingUnSubscriptions;
    std::queue<std::pair<std::string, std::string>> _pendingPublishes;
    std::queue<std::pair<StringGenerator, StringGenerator>> _pendingGenerators;

    // Use a pre-allocated stream for every topic and payload
    std::stringstream _topicGeneratorStream;
    std::stringstream _payloadGeneratoreStream;

    void onClientCallback(const char* topic, const uint8_t* payload, const unsigned int length);
};