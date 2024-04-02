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

    explicit MqttVariableBase(
        PGM_P stateTopic,
        PGM_P commandTopic,
        std::size_t index,
        MqttClient& client
    );

    explicit MqttVariableBase(
        std::string_view topicPrefix,
        PGM_P stateTopic,
        PGM_P commandTopic,
        std::size_t index,
        MqttClient& client
    );

    virtual ~MqttVariableBase();

    MqttVariableBase(const MqttVariableBase&) = delete;
    MqttVariableBase(MqttVariableBase&&) = delete;
    MqttVariableBase& operator=(const MqttVariableBase&) = delete;
    MqttVariableBase& operator=(MqttVariableBase&&) = delete;

    virtual void updateWithPayload(const std::string& payload) = 0;
    virtual void publish() = 0;

    bool needsPublishing() const;

    std::string commandTopic() const;

protected:
    bool _needsPublishing{ true };

    bool publishState(const std::string& payload);

private:
    MqttClient& _client;
    PGM_P const _stateTopic;
    PGM_P const _commandTopic{};
    std::string_view _topicPrefix;
    const std::size_t _index{};

    [[nodiscard]] std::string decorateTopicWithPrefixAndIndex(const std::string& topic) const;
};