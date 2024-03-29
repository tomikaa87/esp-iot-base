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
{

#if defined IOT_ENABLE_MQTT_EXTRA_LARGE_BUFFER
    _client.setBufferSize(1024);
#elif defined IOT_ENABLE_MQTT_LARGE_BUFFER
    _client.setBufferSize(512);
#endif
}

void MqttClient::task()
{
    if (_appConfig.mqtt.enabled) {
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
                    const auto topic = Utils::pgmToStdString(v.topic);
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

                for (auto& v : _variables) {
                    if (v.base->needsPublishing()) {
                        v.base->publish();
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
            }
        }

        _client.loop();
    }
}

bool MqttClient::publish(PGM_P const topic, const std::string& payload, bool dropWhenNotConnected)
{
    return publish(Utils::pgmToStdString(topic), payload, dropWhenNotConnected);
}

bool MqttClient::publish(const std::string& topic, const std::string& payload, bool dropWhenNotConnected)
{
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

void MqttClient::subscribe(PGM_P const topic, MqttVariableBase* const base)
{
    const auto sTopic = Utils::pgmToStdString(topic);

    _log.debug_P(PSTR("subscribe: topic=%s, base=%p"), sTopic.c_str(), base);

    _variables.emplace_back(topic, base);
}

void MqttClient::unsubscribe(PGM_P const topic, MqttVariableBase* const base)
{
    auto sTopic = Utils::pgmToStdString(topic);

    _log.debug_P(PSTR("unsubscribe: topic=%s, base=%p"), sTopic.c_str(), base);

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
    _log.debug_P(PSTR("onClientCallback: topic=%s, payload=%.*s length=%u"), topic, length, payload, length);

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

    _log.debug_P(PSTR("onClientCallback: updating variable, base=%p"), variable->base);

    variable->base->updateWithPayload(
        std::string{ reinterpret_cast<const char*>(payload), length }
    );
}