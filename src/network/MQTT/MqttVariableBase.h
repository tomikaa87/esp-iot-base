#pragma once

#include <cstdint>
#include <string>

class MqttClient;

class MqttVariableBase
{
public:
    explicit MqttVariableBase(const char* topic, MqttClient& client);
    virtual ~MqttVariableBase();

    virtual void updateWithPayload(const std::string& payload) = 0;

    const char* topic() const;

protected:
    MqttClient& _client;

private:
    const char* const _topic;
};