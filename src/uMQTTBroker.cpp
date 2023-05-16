
#include "uMQTTBroker.h"

uMQTTBroker *uMQTTBroker::TheBroker;

    bool uMQTTBroker::_onConnect(struct _clientcon *pclient_conn, uint16_t client_count) {
	return TheBroker->onConnect(pclient_conn->client->remoteIP(), client_count);
   }

    void uMQTTBroker::_onDisconnect(struct _clientcon *pclient_conn, const char *client_id) {
	TheBroker->onDisconnect(pclient_conn->client->remoteIP(), (String)client_id);
    }

    bool uMQTTBroker::_onAuth(const char* username, const char *password, const char* client_id, struct _clientcon *pclient_conn) {
    return TheBroker->onAuth((String)username, (String)password, (String)client_id,pclient_conn);
    }

    void uMQTTBroker::_onData(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t length) {
	char topic_str[topic_len+1];

	os_memcpy(topic_str, topic, topic_len);
	topic_str[topic_len] = '\0';
	TheBroker->onData((String)topic_str, data, length);
    }

    void uMQTTBroker::onRetain(retained_entry *topic){
      save_retainedtopics();
    }

    uMQTTBroker::uMQTTBroker(uint16_t portno, uint16_t max_subscriptions, uint16_t max_retained_topics) {
	TheBroker = this;
	_portno = portno;
	_max_subscriptions = max_subscriptions;
	_max_retained_topics = max_retained_topics;

	MQTT_server_onConnect(_onConnect);
    MQTT_server_onDisconnect(_onDisconnect);
	MQTT_server_onAuth(_onAuth);
	MQTT_server_onData(_onData);
    MQTT_server_onRetain(uMQTTBroker::onRetain);
    }

    void uMQTTBroker::init() {
	MQTT_server_start(_portno, _max_subscriptions, _max_retained_topics);
    }
    void uMQTTBroker::init(uint16_t portno_TLS,const char *pCert,const char *pKey,const char *pCaCert) {
        _portno_TLS=portno_TLS;
        MQTT_server_start(_portno, _max_subscriptions, _max_retained_topics,_portno_TLS,pCert,pKey,pCaCert);
    }
    
    bool uMQTTBroker::onConnect(IPAddress addr, uint16_t client_count) {
	return true;
    }

    void uMQTTBroker::onDisconnect(IPAddress addr, String client_id) {
	return;
    }

    bool uMQTTBroker::onAuth(String username, String password, String client_id,struct _clientcon *pclient_conn) {
	return true;
    }

    void uMQTTBroker::onData(String topic, const char *data, uint32_t length) {
    }

    bool uMQTTBroker::publish(String topic, uint8_t* data, uint16_t data_length, uint8_t qos, uint8_t retain) {
	return MQTT_local_publish((uint8_t*)topic.c_str(), data, data_length, qos, retain);
    }

    uint16_t uMQTTBroker::getClientCount() {
    return MQTT_server_countClientCon();
    }

    bool uMQTTBroker::getClientId(uint16_t index, String &client_id) {
        const char *c = MQTT_server_getClientId(index);
        if (c == NULL)
            return false;
        client_id = c;
        return true;
    }

    bool uMQTTBroker::getClientAddr(uint16_t index, IPAddress& addr) {
        const struct _clientcon* pclient_conn = MQTT_server_getClientPcon(index);
        if (pclient_conn == NULL)
            return false;
        addr = pclient_conn->client->remoteIP();
        return true;
    }

    bool uMQTTBroker::publish(String topic, String data, uint8_t qos, uint8_t retain) {
	return MQTT_local_publish((uint8_t*)topic.c_str(), (uint8_t*)data.c_str(), data.length(), qos, retain);
    }

    bool uMQTTBroker::subscribe(String topic, uint8_t qos) {
	return MQTT_local_subscribe((uint8_t*)topic.c_str(), qos);
    }

    bool uMQTTBroker::unsubscribe(String topic) {
	return MQTT_local_unsubscribe((uint8_t*)topic.c_str());
    }

    void uMQTTBroker::cleanupClientConnections() {
	MQTT_server_cleanupClientCons();
    }

    void uMQTTBroker::loop(){
        MQTT_network_loop();
    }
