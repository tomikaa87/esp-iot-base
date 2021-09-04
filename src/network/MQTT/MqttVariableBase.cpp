#include "MqttVariableBase.h"
#include "MqttClient.h"

MqttVariableBase::MqttVariableBase(
    PGM_P stateTopic,
    MqttClient& client
)
    : MqttVariableBase(stateTopic, nullptr, client)
{}

MqttVariableBase::MqttVariableBase(
    PGM_P stateTopic,
    PGM_P commandTopic,
    MqttClient& client
)
    : _client(client)
    , _stateTopic(stateTopic)
    , _commandTopic(commandTopic)
{
    if (_commandTopic) {
        _client.subscribe(_commandTopic, this);
    }
}

MqttVariableBase::~MqttVariableBase()
{
    if (_commandTopic) {
        _client.unsubscribe(_commandTopic, this);
    }
}

PGM_P MqttVariableBase::stateTopic() const
{
    return _stateTopic;
}

PGM_P MqttVariableBase::commandTopic() const
{
    return _commandTopic;
}