#!/bin/bash
#./generate_certs your-ca-name your-server-name
cat > certs_ca.conf <<EOF
[ req ]
distinguished_name = req_distinguished_name
prompt = no
[ req_distinguished_name ]
O = $1
CN = $1
EOF
cat > certs_server.conf <<EOF
[ req ]
distinguished_name = req_distinguished_name
prompt = no
[ req_distinguished_name ]
O = $2
CN = $2
EOF
openssl ecparam -name prime256v1 -genkey -noout -out ca.key
openssl req -new -x509 -sha256 -key ca.key -out ca.crt
openssl ecparam -name prime256v1 -genkey -noout -out server.key
openssl req -new -sha256 -key server.key -out server.csr
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 5000 -sha256
openssl ecparam -name prime256v1 -genkey -noout -out client1.key
openssl req -new -sha256 -key client1.key -out client1.csr
openssl x509 -req -in client1.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client1.crt -days 5000 -sha256
openssl verify -CAfile ca.crt server.crt client1.crt