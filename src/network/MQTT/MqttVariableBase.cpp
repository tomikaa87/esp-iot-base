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
    _client.registerVariable(this);
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
    _client.registerVariable(this);
}

MqttVariableBase::MqttVariableBase(
    PGM_P stateTopic,
    PGM_P commandTopic,
    const std::size_t index,
    MqttClient& client
)
    : _client(client)
    , _stateTopic(stateTopic)
    , _commandTopic(commandTopic)
    , _index{ index }
{
    _client.registerVariable(this);
}

MqttVariableBase::MqttVariableBase(
    std::string_view topicPrefix,
    PGM_P stateTopic,
    PGM_P commandTopic,
    const std::size_t index,
    MqttClient &client
)
    : _client(client)
    , _stateTopic(stateTopic)
    , _commandTopic(commandTopic)
    , _topicPrefix(std::move(topicPrefix))
    , _index{ index }
{
    _client.registerVariable(this);
}

MqttVariableBase::~MqttVariableBase()
{
    _client.unregisterVariable(this);
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

    return decorateTopicWithPrefixAndIndex(Utils::pgmToStdString(_commandTopic));
}

bool MqttVariableBase::publishState(const std::string& payload)
{
    return _client.publish(
        decorateTopicWithPrefixAndIndex(Utils::pgmToStdString(_stateTopic)),
        payload
    );
}

std::string MqttVariableBase::decorateTopicWithPrefixAndIndex(const std::string& topic) const
{
    std::string result;
    result.reserve(_topicPrefix.size() + topic.size() + 1 /* / */ + 11 /* INT_MAX+1 numbers */);

    if (!_topicPrefix.empty()) {
        result += _topicPrefix;
    }

    result += topic;

    if (_index > 0) {
        result += '/' + std::to_string(_index);
    }

    return result;
}
