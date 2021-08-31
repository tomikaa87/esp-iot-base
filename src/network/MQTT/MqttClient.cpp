#include "MqttClient.h"
#include "MqttVariableBase.h"

#include <PubSubClient.h>

#include <algorithm>
#include <string>

MqttClient::MqttClient(const ApplicationConfig& appConfig)
    : _appConfig(appConfig)
    , _client(
        _appConfig.mqtt.brokerIp,
        _appConfig.mqtt.brokerPort,
        [this](const char* topic, const uint8_t* payload, const unsigned int length) {
            onClientCallback(topic, payload, length);
        },
        _wifiClient
    )
{
}

void MqttClient::task()
{
    if (_appConfig.mqtt.enabled) {
        const auto currentTime = millis();

        if (!_client.connected() && currentTime - _lastConnectAttemptTime >= 5000) {
            _lastConnectAttemptTime = currentTime;

            _log.info("attempting to connect");

            if (_client.connect(_appConfig.mqtt.id)) {
                _lastUpdateTime = 0;

                _log.info("connected");

                _client.publish("esp/test1", "hello");
                _client.subscribe("esp/input1");
            }
        }

        if (_client.connected() && currentTime - _lastUpdateTime >= 2000) {
            _lastUpdateTime = currentTime;

            _client.publish("esp/test1", ("currentTime=" + std::to_string(currentTime)).c_str());
        }

        _client.loop();
    }
}

void MqttClient::publish(const char* topic, const char* payload)
{
    if (_client.connected()) {
        _client.publish(topic, payload);
    }
}

void MqttClient::subscribe(const char* const topic, MqttVariableBase* const base)
{
    _log.debug("subscribe: topic=%s, base=%p", topic, base);

    _variables.emplace_back(topic, base);
}

void MqttClient::unsubscribe(const char* const topic, MqttVariableBase* const base)
{
    _log.debug("unsubscribe: topic=%s, base=%p", topic, base);

    _variables.erase(
        std::remove_if(
            std::begin(_variables),
            std::end(_variables),
            [topic, base](const Variable& v) {
                return
                    strcasecmp(v.topic, topic) == 0
                    && v.base == base;
            }
        ),
        std::end(_variables)
    );
}

void MqttClient::onClientCallback(const char* const topic, const uint8_t* const payload, const unsigned int length)
{
    _log.debug("onClientCallback: topic=%s, payload=%.*s length=%u", topic, length, payload, length);

    const auto variable = std::find_if(
        std::begin(_variables),
        std::end(_variables),
        [topic](const Variable& v) {
            return strcasecmp(v.topic, topic) == 0;
        }
    );

    if (variable == std::end(_variables) || !variable->base) {
        return;
    }

    _log.debug("onClientCallback: updating variable, base=%p", variable->base);

    variable->base->updateWithPayload(
        std::string{ reinterpret_cast<const char*>(payload), length }
    );
}