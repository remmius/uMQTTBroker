/*
 * uMQTTBroker demo for Arduino (C++-style)
 * 
 * The program defines a custom broker class with callbacks, 
 * starts it, subscribes locally to anything, and publishs a topic every second.
 * Try to connect from a remote client and publish something - the console will show this as well.
 */

#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"

/*
 * Your WiFi config here
 */
char ssid[] = "";      // your network SSID (name)
char pass[] =  ""; // your network password
bool WiFiAP = false;      // Do yo want the ESP as AP?

/*
 * Custom broker class with overwritten callback functions
 */
class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString()+" connected");
      return true;
    }

    virtual void onDisconnect(IPAddress addr, String client_id) {
      Serial.println(addr.toString()+" ("+client_id+") disconnected");
    }

    virtual bool onAuth(String username, String password, String client_id) {
      Serial.println("Username/Password/ClientId: "+username+"/"+password+"/"+client_id);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      Serial.println("received topic '"+topic+"' with data '"+(String)data_str+"'");
      //printClients();
    }

    // Sample for the usage of the client info methods

    virtual void printClients() {
      for (int i = 0; i < getClientCount(); i++) {
        IPAddress addr;
        String client_id;
         
        getClientAddr(i, addr);
        getClientId(i, client_id);
        Serial.println("Client "+client_id+" on addr: "+addr.toString());
      }
    }
};

myMQTTBroker myBroker;

/*
 * WiFi init stuff
 */
String linkLocal, ipv6, ipv4;
#include <AddrList.h>

void startWiFiClient()
{
  Serial.println("Connecting to "+(String)ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  #if LWIP_IPV6
// Warten auf die Globale IPv6
  for (bool configured = false; !configured;) {
    for (auto addr : addrList)
      if (configured = addr.isV6() && !addr.isLocal()) {
        break;
      }
    Serial.print('.');
    delay(500);
  }
  Serial.println("\nconnected IPv6\n");
  for (auto a : addrList) {
    a.isV6() ? a.isLocal() ? linkLocal = a.toString() : ipv6 = a.toString() : ipv4 = a.toString();
  }
//URLs für den Browser ausgeben
  Serial.printf("IPv4 address: http://%s\n", ipv4.c_str());
  Serial.printf("IPv6 local: http://[%s]\n", linkLocal.c_str());
  Serial.printf("IPv6 global: http://[%s]\n", ipv6.c_str());
#else
  Serial.printf("IPV6 ist nicht aktiviert\n");
#endif
}


void startWiFiAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Start WiFi
  if (WiFiAP)
    startWiFiAP();
  else
    startWiFiClient();

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();

/*
 * Subscribe to anything
 */
  myBroker.subscribe("#");
}

int counter = 0;
static const unsigned long REFRESH_INTERVAL = 1000; // ms
static unsigned long lastRefreshTime = 0;

void loop()
{
  myBroker.loop(); 
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    myBroker.publish("broker/counter", (String)counter++);
    Serial.println(ESP.getFreeHeap());
    lastRefreshTime = millis();
  }

}
