#!/bin/bash
#./Broker_Load.sh <BROKER-IP>
counter=1
while [ $counter -le 4 ]
do
mosquitto_pub -h $1 -t "test" -m "($counter)" -d -i pub$counter &
((counter++))
mosquitto_pub -h $1 -t "test" -m "($counter)" -d -i pub$counter &
((counter++))
done
 echo All done
