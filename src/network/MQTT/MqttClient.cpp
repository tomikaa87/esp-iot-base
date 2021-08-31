#include "MqttClient.h"
#include "MqttVariableBase.h"
#include "Utils.h"

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
{}

void MqttClient::task()
{
    if (_appConfig.mqtt.enabled) {
        const auto currentTime = millis();

        if (!_client.connected() && currentTime - _lastConnectAttemptTime >= 5000) {
            _lastConnectAttemptTime = currentTime;

            _log.info("attempting to connect");

            if (_client.connect(_appConfig.mqtt.id)) {
                _log.info("connected");

                for (auto& v : _variables) {
                    if (v.subscriptionPending) {
                        const auto topic = Utils::pgmToStdString(v.topic);
                        if (_client.subscribe(topic.c_str())) {
                            _log.debug("successfully subscribed: topic=%s", topic.c_str());
                            v.subscriptionPending = false;
                        }
                    }
                }

                for (auto it = std::begin(_pendingUnSubscriptions); it != std::end(_pendingUnSubscriptions);) {
                    if (_client.unsubscribe(it->c_str())) {
                        _log.debug("successfully unsubscribed: topic=%s", it->c_str());
                        it = _pendingUnSubscriptions.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        _client.loop();
    }
}

void MqttClient::publish(PGM_P const topic, const std::string& payload)
{
    const auto sTopic = Utils::pgmToStdString(topic);

    // _log.debug("publish: topic=%s, payload=%s", sTopic.c_str(), payload.c_str());

    if (_client.connected()) {
        _client.publish(sTopic.c_str(), payload.c_str());
    }
}

void MqttClient::subscribe(PGM_P const topic, MqttVariableBase* const base)
{
    const auto sTopic = Utils::pgmToStdString(topic);

    _log.debug("subscribe: topic=%s, base=%p", sTopic.c_str(), base);

    _variables.emplace_back(topic, base);
}

void MqttClient::unsubscribe(PGM_P const topic, MqttVariableBase* const base)
{
    auto sTopic = Utils::pgmToStdString(topic);

    _log.debug("unsubscribe: topic=%s, base=%p", sTopic.c_str(), base);

    _variables.erase(
        std::remove_if(
            std::begin(_variables),
            std::end(_variables),
            [&sTopic, base](const Variable& v) {
                return
                    strcasecmp_P(sTopic.c_str(), v.topic) == 0
                    && v.base == base;
            }
        ),
        std::end(_variables)
    );

    _pendingUnSubscriptions.push_back(std::move(sTopic));
}

void MqttClient::onClientCallback(const char* const topic, const uint8_t* const payload, const unsigned int length)
{
    _log.debug("onClientCallback: topic=%s, payload=%.*s length=%u", topic, length, payload, length);

    const auto variable = std::find_if(
        std::begin(_variables),
        std::end(_variables),
        [topic](const Variable& v) {
            return strcasecmp_P(topic, v.topic) == 0;
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