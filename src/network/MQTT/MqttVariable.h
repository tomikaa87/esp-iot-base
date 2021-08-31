#pragma once

#include "MqttClient.h"
#include "MqttVariableBase.h"

#include <string>
#include <type_traits>

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
    return false;
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

    explicit MqttVariable(const char* topic, MqttClient& client)
        : MqttVariableBase(topic, client)
    {}

    MqttVariable& operator=(ValueType&& v) {
        _value = std::forward<ValueType>(v);
        using namespace std;
        _client.publish(topic(), to_string(_value));
        return *this;
    }

    explicit operator const ValueType&() const {
        return _value;
    }

private:
    ValueType _value{};

    void updateWithPayload(const std::string& payload) override {
        _value = from_payload<ValueType>(payload);
    }
};