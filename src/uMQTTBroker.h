#ifndef _MQTT_BROKER_H_
#define _MQTT_BROKER_H_

#include "user_interface.h"
#include "IPAddress.h"
#include "string.h"

#ifndef ipv4_addr_t
#define ipv4_addr_t ip_addr_t
#endif

#include "mqtt/mqtt_server.h"
/*
extern "C" {
#include "mqtt/mqtt_retainedlist.h"
void ICACHE_FLASH_ATTR MQTT_ClientCon_connected_cb(void *arg);
void ICACHE_FLASH_ATTR MQTT_ClientCon_sent_cb(void *arg);
void ICACHE_FLASH_ATTR MQTT_ClientCon_discon_cb(void *arg);
void ICACHE_FLASH_ATTR MQTT_ClientCon_recv_cb(void *arg, char *pdata, unsigned short len);

// Interface for starting the broker

bool MQTT_server_start(uint16_t portno, uint16_t max_subscriptions, uint16_t max_retained_topics);

// Callbacks for message reception, username/password authentication, and client connection

typedef void (*MqttDataCallback)(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t lengh);
typedef bool (*MqttAuthCallback)(const char* username, const char *password, const char *client_id, struct espconn *pesp_conn);
typedef bool (*MqttConnectCallback)(struct espconn *pesp_conn, uint16_t client_count);
typedef void (*MqttDisconnectCallback)(struct espconn *pesp_conn, const char *client_id);

void MQTT_server_onData(MqttDataCallback dataCb);
void MQTT_server_onAuth(MqttAuthCallback authCb);
void MQTT_server_onConnect(MqttConnectCallback connectCb);
void MQTT_server_onDisconnect(MqttDisconnectCallback disconnectCb);

// Interface for local pub/sub interaction with the broker

bool MQTT_local_publish(uint8_t* topic, uint8_t* data, uint16_t data_length, uint8_t qos, uint8_t retain);
bool MQTT_local_subscribe(uint8_t* topic, uint8_t qos);
bool MQTT_local_unsubscribe(uint8_t* topic);

// Interface to cleanup after STA disconnect

void MQTT_server_cleanupClientCons();

// Interface for persistence of retained topics
// Topics can be serialized to a buffer and reinitialized later after reboot
// Application is responsible for saving and restoring that buffer (i.e. to/from flash)

void clear_retainedtopics();
int serialize_retainedtopics(char *buf, int len);
bool deserialize_retainedtopics(char *buf, int len);

// Interface for getting some infos on the currently connected clients
// MQTT_server_getClientId() and MQTT_server_getClientPcon() return NULL on invalid indices

uint16_t MQTT_server_countClientCon();
const char* MQTT_server_getClientId(uint16_t index);
const struct _myclientcon* MQTT_server_getClientPcon(uint16_t index);
}
*/
class uMQTTBroker
{
private:
    static uMQTTBroker *TheBroker;
    uint16_t _portno;
    #ifdef MQTT_TLS_ON
    uint16_t _portno_TLS;
    #endif
    uint16_t _max_subscriptions;
    uint16_t _max_retained_topics;

    static bool _onConnect(struct _myclientcon *pesp_conn, uint16_t client_count);
    static void _onDisconnect(struct _myclientcon *pesp_conn, const char *client_id);
    static bool _onAuth(const char* username, const char *password,  const char *client_id, struct _myclientcon *pesp_conn);
    static void _onData(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t length);

public:
    uMQTTBroker(uint16_t portno=1883, uint16_t max_subscriptions=30, uint16_t max_retained_topics=30);

    void init();
    #ifdef MQTT_TLS_ON
    void init(uint16_t portno_TLS);
    #endif    
    void loop();

// Callbacks on client actions

    virtual bool onConnect(IPAddress addr, uint16_t client_count);
    virtual void onDisconnect(IPAddress addr, String client_id);
    virtual bool onAuth(String username, String password, String client_id,struct _myclientcon *pesp_conn);
    virtual void onData(String topic, const char *data, uint32_t length);
    static void onRetain(struct _retained_entry *topic);
// Infos on currently connected clients

    virtual uint16_t getClientCount();
    virtual bool getClientId(uint16_t index, String &client_id);
    virtual bool getClientAddr(uint16_t index, IPAddress& addr);

// Interaction with the local broker

    virtual bool publish(String topic, uint8_t* data, uint16_t data_length, uint8_t qos=0, uint8_t retain=0);
    virtual bool publish(String topic, String data, uint8_t qos=0, uint8_t retain=0);
    virtual bool subscribe(String topic, uint8_t qos=0);
    virtual bool unsubscribe(String topic);

// Cleanup all clients on Wifi connection loss

    void cleanupClientConnections();
};

#endif /* _MQTT_BROKER_H_ */
