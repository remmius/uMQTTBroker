## Added features based on martin-ger/uMQTTBroker project
Replaced esp_conn-functions with WifiClient from ESP8266WiFi.h to enable simple support IP6 broker address, usage of newer core versions/lwip version and TLS-client option.
Callbacks on datasend/datarecieved are not available in WifiClient, therefore a 'ugly' workaround was done looping over the connections for incoming data.

** Notes**
- IP6 support for broker with Arduino v2 IP6 (low memory)flag or build-flag PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_HIGHER_BANDWIDTH
- Add basic TLS support for broker with build-flag MQTT_TLS_ON in defaults.h. In ArduinoIDE set SSL support to all ciphers. Limited to 1 TLS-client due to memory restriction on ESP8266. ~10KB Ram needed with 512  MQTT_BUF_SIZE per client. Client cert-based authentification is not working currently due to BSSL:_wait_for_handshake: failed error but also requires even more Ram/ execution time. Consider runnning ESP with 160MHz. Dont't forget to generate our own certificates, find a example script in the certs folder of the corresponding example!  
- The MQTT-client component still uses esp_conn. To compile with lwip Variant v2, DNS support is simply commented out to resolve miss address members for now. I did not test/use the client. 
- Added Persistance of retain topics with build-flag: MQTT_RETAIN_PERSISTANCE (Retain topics are stored in flash, which has a limited lifetime!)
- No resending/splitting of messages in case client.availableForWrite() is smaller than the mqtt-message. 
- Changes only tested rudimentarily. 
- A better approach (closer to the original esp_conn-functions) would be probably using something like https://github.com/me-no-dev/ESPAsyncTCP (no TLS?) or similar based on network-callbacks.

# uMQTTBroker
MQTT Broker library for ESP8266 Arduino

You can start an MQTT broker in any ESP Arduino project. Just clone (or download the zip-file and extract it) into the libraries directory of your Arduino ESP8266 installation.

**Important: Use the setting "lwip Variant: 1.4 High Bandwidth" in the "Tools" menu**

lwip 2.0 has some strange behaviour that causes the socket to block after 5 connections.

Thanks to Tuan PM for sharing his MQTT client library https://github.com/tuanpmt/esp_mqtt as a basis with us. The modified code still contains the complete client functionality from the original esp_mqtt lib, but it has been extended by the basic broker service.

The broker does support:
- MQTT protocoll versions v3.1 and v3.1.1 simultaniously
- a smaller number of clients (at least 8 have been tested, memory is the issue)
- retained messages
- LWT
- QoS level 0
- username/password authentication
 
The broker does not yet support:
- QoS levels other than 0
- many TCP(=MQTT) clients
- non-clear sessions
- TLS

If you are searching for a complete ready-to-run MQTT broker for the ESP8266 with additional features (persistent configuration, scripting support and much more) have a look at https://github.com/martin-ger/esp_mqtt .

## API MQTT Broker (C++-style)
The MQTT broker has a new C++ style API with a broker class:
```c
class uMQTTBroker
{
public:
    uMQTTBroker(uint16_t portno=1883, uint16_t max_subscriptions=30, uint16_t max_retained_topics=30);

    void init();

// Callbacks on client actions

    virtual bool onConnect(IPAddress addr, uint16_t client_count);
    virtual void onDisconnect(IPAddress addr, String client_id);
    virtual bool onAuth(String username, String password, String client_id);
    virtual void onData(String topic, const char *data, uint32_t length);

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
```
Use the broker as shown in oo-examples found in https://github.com/martin-ger/uMQTTBroker/tree/master/examples .

## API MQTT Broker (C-style)
The MQTT broker is started by simply including:

```c
#include "uMQTTBroker.h"
```
and then calling
```c
bool MQTT_server_start(uint16_t portno, uint16_t max_subscriptions, uint16_t max_retained_topics);
```
in the "setup()" function. Now it is ready for MQTT connections on all activated interfaces (STA and/or AP). The MQTT server will run in the background and you can connect with any MQTT client. Your Arduino project might do other application logic in its loop.

Your code can locally interact with the broker using these functions:

```c
bool MQTT_local_publish(uint8_t* topic, uint8_t* data, uint16_t data_length, uint8_t qos, uint8_t retain);
bool MQTT_local_subscribe(uint8_t* topic, uint8_t qos);
bool MQTT_local_unsubscribe(uint8_t* topic);


void MQTT_server_onData(MqttDataCallback dataCb);
```

With these functions you can publish and subscribe topics as a local client like you would with any remote MQTT broker. The provided dataCb is called on each reception of a matching topic, no matter whether it was published from a remote client or the "MQTT_local_publish()" function.

Username/password authentication is provided with the following interface:

```c
typedef bool (*MqttAuthCallback)(const char* username, const char *password, const char *client_id, struct espconn *pesp_conn);

void MQTT_server_onAuth(MqttAuthCallback authCb);

typedef bool (*MqttConnectCallback)(struct espconn *pesp_conn, uint16_t client_count);

void MQTT_server_onConnect(MqttConnectCallback connectCb);

typedef void (*MqttDisconnectCallback)(struct espconn *pesp_conn, const char *client_id);

void MQTT_server_onDisconnect(MqttDisconnectCallback disconnectCb);
```

If an *MqttAuthCallback* function is registered with MQTT_server_onAuth(), it is called on each connect request. Based on username, password, and optionally the connection info (the client Id and the IP address) the function has to return *true* for authenticated or *false* for rejected. If a request provides no username and/or password these parameter strings are empty. If no *MqttAuthCallback* function is set, each request will be admitted.

The *MqttConnectCallback* function does a similar check for the connection, but it is called right after the connect request before the MQTT connect request is processed. This is done in order to reject requests from unautorized clients in an early stage. The number of currently connected clients (incl. the current one) is given in the *client_count* paramater. With this info you can reject too many concurrent connections.

The *MqttDisconnectCallback* is called each time a client disconnects from the server.

If you want to force a cleanup when the broker as a WiFi client (WIFI_STA mode) has lost connectivity to the AP, call:
```c
void MQTT_server_cleanupClientCons();
```
This will remove all broken connections, publishing LWT if defined.

Sample: in the Arduino setup() initialize the WiFi connection (client or SoftAP, whatever you need) and somewhere at the end add these line:
```c
MQTT_server_start(1883, 30, 30);
```

You can find a sample sketch here https://github.com/martin-ger/uMQTTBroker/tree/master/examples .

## API MQTT Client

To use the MQTT client functionality include:

```c
#include "MQTT.h"
```

This code is taken from Ingo Randolf from esp-mqtt-arduino (https://github.com/i-n-g-o/esp-mqtt-arduino). It is a wrapper to tuanpmt's esp_mqtt client library. Look here https://github.com/i-n-g-o/esp-mqtt-arduino/tree/master/examples for code samples.

# Thanks
- tuanpmt for esp_mqtt (https://github.com/tuanpmt/esp_mqtt )
- Ingo Randolf for esp-mqtt-arduino (https://github.com/i-n-g-o/esp-mqtt-arduino)
- Ian Craggs for mqtt_topic
- many others contributing to open software (for the ESP8266)
