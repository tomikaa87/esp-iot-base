#include "MqttVariableBase.h"
#include "MqttClient.h"

MqttVariableBase::MqttVariableBase(const char* topic, MqttClient& client)
    : _client(client)
    , _topic(topic)
{
    _client.subscribe(_topic, this);
}

MqttVariableBase::~MqttVariableBase()
{
    _client.unsubscribe(_topic, this);
}

const char* MqttVariableBase::topic() const
{
    return _topic;
}