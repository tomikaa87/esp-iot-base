#pragma once

#include <cstdint>
#include <string>

#include <pgmspace.h>

class MqttClient;

class MqttVariableBase
{
public:
    MqttVariableBase(
        PGM_P pstrStateTopic,
        MqttClient& client
    );

    MqttVariableBase(
        PGM_P stateTopic,
        PGM_P commandTopic,
        MqttClient& client
    );

    virtual ~MqttVariableBase();

    virtual void updateWithPayload(const std::string& payload) = 0;

    PGM_P stateTopic() const;
    PGM_P commandTopic() const;

protected:
    MqttClient& _client;

private:
    PGM_P const _stateTopic;
    PGM_P const _commandTopic = nullptr;
};