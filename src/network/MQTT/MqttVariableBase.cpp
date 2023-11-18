#include "MqttVariableBase.h"
#include "MqttClient.h"
#include "Utils.h"

MqttVariableBase::MqttVariableBase(
    PGM_P stateTopic,
    MqttClient& client
)
    : MqttVariableBase(stateTopic, nullptr, client)
{}

MqttVariableBase::MqttVariableBase(
    std::string_view topicPrefix,
    PGM_P stateTopic,
    MqttClient &client
)
    : MqttVariableBase(std::move(topicPrefix), stateTopic, nullptr, client)
{
}

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
        _client.subscribeToCommandTopic(this);
    }
}

MqttVariableBase::MqttVariableBase(
    std::string_view topicPrefix,
    PGM_P stateTopic,
    PGM_P commandTopic,
    MqttClient &client
)
    : _client(client)
    , _stateTopic(stateTopic)
    , _commandTopic(commandTopic)
    , _topicPrefix(std::move(topicPrefix))
{
    if (_commandTopic) {
        _client.subscribeToCommandTopic(this);
    }
}

MqttVariableBase::~MqttVariableBase()
{
    if (_commandTopic) {
        _client.unsubscribeFromCommandTopic(this);
    }
}

bool MqttVariableBase::needsPublishing() const
{
    return _needsPublishing;
}

std::string MqttVariableBase::commandTopic() const
{
    if (!_commandTopic) {
        return{};
    }

    const auto topic = Utils::pgmToStdString(_commandTopic);

    if (!_topicPrefix.empty()) {
        return std::string{ _topicPrefix } + topic;
    }

    return topic;
}

bool MqttVariableBase::publishState(const std::string& payload)
{
    if (!_topicPrefix.empty()) {
        return _client.publish(
            std::string{ _topicPrefix } + Utils::pgmToStdString(_stateTopic),
            payload
        );
    } else {
        return _client.publish(_stateTopic, payload);
    }
}
