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
    if (!_appConfig.mqtt.enabled) {
        return;
    }

    const auto currentTime = millis();

    if (!_client.connected() && currentTime - _lastConnectAttemptTime >= 5000) {
        _lastConnectAttemptTime = currentTime;

        _log.info(
            "attempting to connect: brokerIp=%s, brokerPort=%u",
            _appConfig.mqtt.brokerIp.toString().c_str(),
            _appConfig.mqtt.brokerPort
        );

        if (_client.connect(_appConfig.mqtt.id, _appConfig.mqtt.user, _appConfig.mqtt.password)) {
            _log.info_P(PSTR("connected"));

            for (auto& v : _variables) {
                const auto topic = v->commandTopic();
                if (topic.empty()) {
                    continue;
                }
                if (_client.subscribe(topic.c_str())) {
                    _log.debug_P(PSTR("successfully subscribed: topic=%s"), topic.c_str());
                }
            }

            for (auto it = std::begin(_pendingUnSubscriptions); it != std::end(_pendingUnSubscriptions);) {
                if (_client.unsubscribe(it->c_str())) {
                    _log.debug_P(PSTR("successfully unsubscribed: topic=%s"), it->c_str());
                    it = _pendingUnSubscriptions.erase(it);
                } else {
                    ++it;
                }
            }

            for (auto* v : _variables) {
                if (v->needsPublishing()) {
                    v->publish();
                }
            }

            while (!_pendingPublishes.empty()) {
                const auto& p = _pendingPublishes.front();

                _log.debug_P(PSTR("task: publishing pending item, topic=%s, payload=%s"), p.first.c_str(), p.second.c_str());

                if (_client.publish(p.first.c_str(), p.second.c_str(), true)) {
                    _pendingPublishes.pop();
                } else {
                    _log.warning_P(PSTR("task: failed to publish pending item"));
                    break;
                }
            }

            while (!_pendingGenerators.empty()) {
                const auto& p = _pendingGenerators.front();

                const auto topic = p.first();
                const auto payload = p.second();

                _log.debug_P(PSTR("task: publishing pending item, topic=%s, payload=%s"), topic.c_str(), payload.c_str());

                if (_client.publish(topic.c_str(), payload.c_str(), true)) {
                    _pendingGenerators.pop();
                } else {
                    _log.warning_P(PSTR("task: failed to publish pending item"));
                    break;
                }
            }
        }
    }

    _client.loop();
}

bool MqttClient::publish(PGM_P const topic, const std::string& payload, bool dropWhenNotConnected)
{
    return publish(Utils::pgmToStdString(topic), payload, dropWhenNotConnected);
}

bool MqttClient::publish(const std::string& topic, const std::string& payload, bool dropWhenNotConnected)
{
    if (!_appConfig.mqtt.enabled) {
        return false;
    }

    _log.debug_P(PSTR("publish: topic=%s, payload=%s"), topic.c_str(), payload.c_str());

    if (_client.connected()) {
        return _client.publish(topic.c_str(), payload.c_str(), true);
    } else {
        if (dropWhenNotConnected) {
            _log.warning_P(PSTR("publish: failed, client not connected"));
        } else {
            _log.warning_P(PSTR("publish: pending, client not connected"));
            _pendingPublishes.push(std::make_pair(topic, payload));
        }
        return false;
    }
}

bool MqttClient::publish(StringGenerator&& topic, StringGenerator&& payload)
{
    if (!_appConfig.mqtt.enabled) {
        return false;
    }

    if (_client.connected()) {
        return _client.publish(topic().c_str(), payload().c_str());
    } else {
        _pendingGenerators.emplace(std::move(topic), std::move(payload));
        return false;
    }
}

void MqttClient::registerVariable(MqttVariableBase* base)
{
    // _log.debug_P(PSTR("registerVariable: commandTopic=%s, base=%p"), base->commandTopic().c_str(), base);

    _variables.emplace_back(base);
}

void MqttClient::unregisterVariable(MqttVariableBase* base)
{
    // _log.debug_P(PSTR("unregisterVariable: commandTopic=%s, base=%p"), base->commandTopic().c_str(), base);

    _variables.erase(
        std::remove_if(
            std::begin(_variables),
            std::end(_variables),
            [base](const MqttVariableBase* v) {
                return v == base;
            }
        ),
        std::end(_variables)
    );

    auto commandTopic{ std::move(base->commandTopic()) };

    if (!commandTopic.empty()) {
        _pendingUnSubscriptions.push_back(std::move(commandTopic));
    }
}

bool MqttClient::isConnected() const
{
    return _client.state() == MQTT_CONNECTED;
}

void MqttClient::onClientCallback(const char* const topic, const uint8_t* const payload, const unsigned int length)
{
    _log.debug_P(PSTR("onClientCallback: topic=%s, payload=%.*s, length=%u"), topic, length, payload, length);

    const auto variable = std::find_if(
        std::begin(_variables),
        std::end(_variables),
        [topic](const MqttVariableBase* v) {
            return v->commandTopic() == topic;
        }
    );

    if (variable == std::end(_variables)) {
        return;
    }

    _log.debug_P(PSTR("onClientCallback: updating variable, base=%p"), variable);

    (*variable)->updateWithPayload(
        std::string{ reinterpret_cast<const char*>(payload), length }
    );
}