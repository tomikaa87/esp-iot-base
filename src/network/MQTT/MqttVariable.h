#pragma once

#include "MqttClient.h"
#include "MqttVariableBase.h"

#include <string>
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

    MqttVariable(
        PGM_P stateTopic,
        MqttClient& client
    )
        : MqttVariableBase(stateTopic, client)
    {}

    MqttVariable(
        PGM_P stateTopic,
        PGM_P commandTopic,
        MqttClient& client
    )
        : MqttVariableBase(stateTopic, commandTopic, client)
    {}

    MqttVariable& operator=(const ValueType& v) {
        if (_value == v) {
            return *this;
        }

        _value = v;

        publish();

        return *this;
    }

    explicit operator const ValueType&() const {
        return _value;
    }

    void setChangedHandler(ChangedHandler&& handler)
    {
        _changedHandler = std::move(handler);
    }

private:
    ValueType _value{};
    ChangedHandler _changedHandler;

    void updateWithPayload(const std::string& payload) override
    {
        _value = from_payload<ValueType>(payload);

        if (_changedHandler) {
            _changedHandler(_value);
        }
    }

    void publish() override
    {
        using namespace std;
        _needsPublishing = !_client.publish(stateTopic(), to_string(_value));
    }
};