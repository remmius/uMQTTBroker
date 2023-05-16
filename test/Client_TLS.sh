#!/bin/bash
#./Client_TLS.sh <BROKER-IP> client-verify

counter=1
path_certs=../examples/uMQTTBrokerSampleOOFull_TLS/certs
openssl verify -CAfile $path_certs/ca.crt $path_certs/server.crt $path_certs/client1.crt
longmsg=$(head -c 400 < /dev/zero | tr '\0' 'A')
if [ "$#" -ne 2 ]
then
  counter=1
  echo "Authentificating without client-certs"
  mosquitto_pub -h $1 -t "test" -d -i pubTLS$counter -m "TLS0$counter_$longmsg" -p 8883 --cafile $path_certs/ca.crt --insecure
  exit 1
fi
if [ "$#" -ne 3 ]
then
  counter=2
  echo "Authentificating with client-certs"
  mosquitto_pub -h $1 -t "test" -d -i pubTLS$counter -m "TLS0$counter_$longmsg" -p 8883 --cafile $path_certs/ca.crt --insecure --key $path_certs/client1.key --cert $path_certs/client1.crt
  exit 1
fi
echo "Provide host as argument"
