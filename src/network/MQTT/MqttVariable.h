#pragma once

#include "MqttClient.h"
#include "MqttVariableBase.h"

#include <ctime>
#include <string>
#include <string_view>
#include <type_traits>

#include <pgmspace.h>

template <typename T>
T from_payload(const std::string& payload);

template <>
inline int from_payload<int>(const std::string& payload)
{
    return strtol(payload.c_str(), nullptr, 10);
}

template <>
inline unsigned from_payload<unsigned>(const std::string& payload)
{
    return strtoul(payload.c_str(), nullptr, 10);
}

template <>
inline float from_payload<float>(const std::string& payload)
{
    return strtof(payload.c_str(), nullptr);
}

template <>
inline double from_payload<double>(const std::string& payload)
{
    return strtod(payload.c_str(), nullptr);
}

template <>
inline bool from_payload<bool>(const std::string& payload)
{
    return from_payload<int>(payload) > 0;
}

template <>
inline std::string from_payload<std::string>(const std::string& payload)
{
    return payload;
}

template <>
inline std::time_t from_payload<std::time_t>(const std::string& payload)
{
    return strtoll(payload.c_str(), nullptr, 10);
}

inline std::string to_string(const std::string& payload)
{
    return payload;
}

template <typename T>
class MqttVariable : MqttVariableBase
{
public:
    using ValueType = typename std::remove_reference<T>::type;
    using ChangedHandler = std::function<void (const ValueType& value)>;

    explicit MqttVariable(
        PGM_P stateTopic,
        MqttClient& client,
        bool publishOnWrite = false
    )
        : MqttVariableBase(stateTopic, client)
        , _publishOnWrite(publishOnWrite)
    {}

    explicit MqttVariable(
        std::string_view topicPrefix,
        PGM_P stateTopic,
        MqttClient& client,
        bool publishOnWrite = false
    )
        : MqttVariableBase(std::move(topicPrefix), stateTopic, client)
        , _publishOnWrite(publishOnWrite)
    {}

    explicit MqttVariable(
        PGM_P stateTopic,
        PGM_P commandTopic,
        MqttClient& client,
        bool publishOnWrite = false
    )
        : MqttVariableBase(stateTopic, commandTopic, client)
        , _publishOnWrite(publishOnWrite)
    {}

    explicit MqttVariable(
        std::string_view topicPrefix,
        PGM_P stateTopic,
        PGM_P commandTopic,
        MqttClient& client,
        bool publishOnWrite = false
    )
        : MqttVariableBase(std::move(topicPrefix), stateTopic, commandTopic, client)
        , _publishOnWrite(publishOnWrite)
    {}

    explicit MqttVariable(
        std::string_view topicPrefix,
        PGM_P stateTopic,
        PGM_P commandTopic,
        std::size_t index,
        MqttClient& client,
        bool publishOnWrite = false
    )
        : MqttVariableBase(std::move(topicPrefix), stateTopic, commandTopic, index, client)
        , _publishOnWrite(publishOnWrite)
    {}

    MqttVariable& operator=(ValueType v) {
        if (!_publishOnWrite && _value == v) {
            return *this;
        }

        _value = std::move(v);

        publish();

        return *this;
    }

    MqttVariable(const MqttVariable&) = delete;
    MqttVariable(MqttVariable&&) = delete;
    MqttVariable& operator=(const MqttVariable&) = delete;
    MqttVariable& operator=(MqttVariable&&) = delete;

    explicit operator const ValueType&() const {
        return _value;
    }

    void setChangedHandler(ChangedHandler&& handler)
    {
        _changedHandler = std::move(handler);
    }

private:
    bool _publishOnWrite = false;
    ValueType _value{};
    ChangedHandler _changedHandler;

    void updateWithPayload(const std::string& payload) override
    {
        auto value = from_payload<ValueType>(payload);

        if (value != _value) {
            _value = std::move(value);

            if (_changedHandler) {
                _changedHandler(_value);
            }
        }

        publish();
    }

    void publish() override
    {
        using namespace std;
        _needsPublishing = !publishState(to_string(_value));
    }
};