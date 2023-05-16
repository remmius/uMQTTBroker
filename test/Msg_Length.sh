#!/bin/bash
#./Clients_Load.sh <BROKER-IP>

for (( counter=200; counter<1024; counter=counter+64))
do
msg=$(head -c $counter < /dev/zero | tr '\0' 'A')
mosquitto_pub -h $1 -t "test" -m "$counter($msg)" -d -i pub$counter
((counter++))
sleep 2
done
 echo All done
