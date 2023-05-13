#!/bin/bash
#./Client_TLS.sh <BROKER-IP>
counter=1
mosquitto_pub -h $1 -t "test" -d -i pubTLS$counter -m "TLS0$counter" -p 8883 --cafile ../examples/uMQTTBrokerSampleOOFull_TLS/certs/ca.crt --insecure
counter=2
sleep 2
#Clients without a certificate are passing
mosquitto_pub -h $1 -t "test" -d -i pubTLS$counter -m "TLS0$counter" -p 8883 --cafile ../examples/uMQTTBrokerSampleOOFull_TLS/certs/ca.crt --insecure --key ../examples/uMQTTBrokerSampleOOFull_TLS/certs/client1.key --cert ../examples/uMQTTBrokerSampleOOFull_TLS/certs/client1.crt