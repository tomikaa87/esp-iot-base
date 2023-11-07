#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <pgmspace.h>

class MqttClient;

class MqttVariableBase
{
public:
    explicit MqttVariableBase(
        PGM_P stateTopic,
        MqttClient& client
    );

    explicit MqttVariableBase(
        std::string_view topicPrefix,
        PGM_P stateTopic,
        MqttClient& client
    );

    explicit MqttVariableBase(
        PGM_P stateTopic,
        PGM_P commandTopic,
        MqttClient& client
    );

    explicit MqttVariableBase(
        std::string_view topicPrefix,
        PGM_P stateTopic,
        PGM_P commandTopic,
        MqttClient& client
    );

    virtual ~MqttVariableBase();

    virtual void updateWithPayload(const std::string& payload) = 0;
    virtual void publish() = 0;

    bool needsPublishing() const;

    std::string commandTopic() const;

protected:
    bool _needsPublishing = false;

    bool publishState(const std::string& payload);

private:
    MqttClient& _client;
    PGM_P const _stateTopic;
    PGM_P const _commandTopic = nullptr;
    std::string_view _topicPrefix;
};