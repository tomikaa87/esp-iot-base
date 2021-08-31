#pragma once

#include <string>

template <typename T>
T from_payload(const std::string& payload);

template <>
int from_payload<int>(const std::string& payload)
{
    return strtol(payload.c_str(), nullptr, 10);
}

template <>
unsigned from_payload<unsigned>(const std::string& payload)
{
    return strtoul(payload.c_str(), nullptr, 10);
}

template <>
float from_payload<float>(const std::string& payload)
{
    return strtof(payload.c_str(), nullptr);
}

template <>
double from_payload<double>(const std::string& payload)
{
    return strtod(payload.c_str(), nullptr);
}

template <>
std::string from_payload<std::string>(const std::string& payload)
{
    return payload;
}

std::string to_string(const std::string& payload)
{
    return payload;
}

template <typename T>
struct MqttVariable : MqttVariableBase {
    using ValueType = typename std::remove_reference_t<T>;

    explicit MqttVariable(std::string topic, MqttClient& client)
        : MqttVariableBase(std::move(topic), client)
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