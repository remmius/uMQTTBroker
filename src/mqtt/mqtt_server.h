#ifndef _MQTT_SERVER_H_
#define _MQTT_SERVER_H_

#include "user_interface.h"
#include "c_types.h"
#include "osapi.h"
#include "os_type.h"
//#include "ip_addr.h"
extern "C" {
	#include "mqtt.h"
  #include "mqtt_retainedlist.h"
}
//#include "espconn.h"
//#include "lwip/ip.h"
//#include "lwip/app/espconn.h"
//#include "lwip/app/espconn_tcp.h"
#include <ESP8266WiFi.h>
typedef struct _myclientcon {
   WiFiClient *client;
   void *reverse; 
 } myclientcon;

#define LOCAL_MQTT_CLIENT ((void*)-1)

void ICACHE_FLASH_ATTR MQTT_ClientCon_connected_cb(void *arg);
void ICACHE_FLASH_ATTR MQTT_ClientCon_sent_cb(void *arg);
void ICACHE_FLASH_ATTR MQTT_ClientCon_discon_cb(void *arg);
void ICACHE_FLASH_ATTR MQTT_ClientCon_recv_cb(void *arg, char *pdata, unsigned short len);


typedef bool (*MqttAuthCallback)(const char* username, const char *password, const char* client_id, struct _myclientcon *pesp_conn);
typedef bool (*MqttConnectCallback)(struct _myclientcon *pesp_conn, uint16_t client_count);
typedef void (*MqttDisconnectCallback)(struct _myclientcon *pesp_conn, const char* client_id);

typedef struct _MQTT_ClientCon {
  struct _myclientcon *pCon;
//  uint8_t security;
//  uint32_t port;
//  ip_addr_t ip;
  mqtt_state_t  mqtt_state;
  mqtt_connect_info_t connect_info;
//  MqttCallback connectedCb;
//  MqttCallback disconnectedCb;
//  MqttCallback publishedCb;
//  MqttCallback timeoutCb;
//  MqttDataCallback dataCb;
  ETSTimer mqttTimer;
  uint32_t sendTimeout;
  uint32_t connectionTimeout;
  tConnState connState;
  QUEUE msgQueue;
  uint8_t protocolVersion;
  void* user_data;
  struct _MQTT_ClientCon *next;
  uint8_t id;
} MQTT_ClientCon;

extern MQTT_ClientCon *clientcon_list;

uint16_t MQTT_server_countClientCon();
const char* MQTT_server_getClientId(uint16_t index);
const struct _myclientcon* MQTT_server_getClientPcon(uint16_t index);

void MQTT_server_disconnectClientCon(MQTT_ClientCon *mqttClientCon);
bool MQTT_server_deleteClientCon(MQTT_ClientCon *mqttClientCon);
void MQTT_server_cleanupClientCons();

bool MQTT_server_start(uint16_t portno, uint16_t max_subscriptions, uint16_t max_retained_topics);
void MQTT_server_onConnect(MqttConnectCallback connectCb);
void MQTT_server_onDisconnect(MqttDisconnectCallback disconnectCb);
void MQTT_server_onAuth(MqttAuthCallback authCb);
void MQTT_server_onData(MqttDataCallback dataCb);
void MQTT_server_onRetain(on_retainedtopic_cb cb);

bool MQTT_local_publish(uint8_t* topic, uint8_t* data, uint16_t data_length, uint8_t qos, uint8_t retain);
bool MQTT_local_subscribe(uint8_t* topic, uint8_t qos);
bool MQTT_local_unsubscribe(uint8_t* topic);

bool publish_retainedtopic(retained_entry * entry, void* user_data);
void MQTT_network_loop();
#endif /* _MQTT_SERVER_H_ */
