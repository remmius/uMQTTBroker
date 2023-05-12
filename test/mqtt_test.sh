#!/bin/bash
#./mqtt_test.sh <BROKER-IP>
counter=1
while [ $counter -le 2 ]
do
mosquitto_pub -h $1 -t "t($counter)" -m "0" -d -i pub$counter &
((counter++))
mosquitto_pub -h $1 -t "t($counter)" -m "1" -d -i pub$counter &
((counter++))
done
 echo All done
