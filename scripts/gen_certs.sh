#!/bin/bash

PRG="$0"
# Need this for relative symlinks.
while [ -h "$PRG" ] ; do
    ls=`ls -ld "$PRG"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '/.*' > /dev/null; then
        PRG="$link"
    else
        PRG=`dirname "$PRG"`"/$link"
    fi
done
SAVED="`pwd`"
cd "`dirname \"$PRG\"`/.." >&-
APP_HOME="`pwd -P`"
cd "$SAVED" >&-


if [ -f "${APP_HOME}/certs/${1}.jks" ]
then
  exit 0
fi

mkdir -p ${APP_HOME}/certs
cd   ${APP_HOME}/certs

# Create the CA that will be known by server and client
openssl  req  -passout pass:changeit -new  -x509  -keyout  server-ca-key.pem -out  server-ca-certificate.pem  -days  365 << EOF
changeit
changeit
US
CA
CA Test For Server Known CA
The Testing CA
Tester CA
localhost
ca@foo.bar
EOF

# Create the CA that will be only known by the client
openssl  req  -passout pass:changeit -new  -x509  -keyout  client-ca-key.pem -out  client-ca-certificate.pem  -days  365 << EOF
changeit
changeit
US
CA
CA Test For Client Only
The Testing CA Client Only
Tester CA
localhost
ca@foo.bar
EOF

#Create the "server key store"
cd ${APP_HOME}/certs

keytool -genkey -alias testing -keystore server.jks -keyalg RSA -sigalg SHA1withRSA << EOF
changeit
changeit
localhost
Test OU
Test OU Name
Testing City
Unit Test
US
yes
changeit
changeit
EOF


keytool -list -v -keystore server.jks -storepass changeit

# Generate the Signing Request For the server
keytool -keystore server.jks -certreq -alias testing -keyalg rsa  -file server.csr -storepass changeit

# Sign the server cert with the CA known by the client and server
openssl  x509  -req  -passin pass:changeit -CA server-ca-certificate.pem -CAkey server-ca-key.pem -in server.csr -out server.cer  -days 365  -CAcreateserial

# Import the two way SSL CA cert.
keytool -import -keystore server.jks -file server-ca-certificate.pem -alias testtrustca -storepass changeit << EOF
y
EOF

#Import our signed cert
keytool -import -keystore server.jks -file server.cer -alias testing -storepass changeit

keytool -list -v -keystore server.jks -storepass changeit

openssl req \
       -newkey rsa:2048 -nodes -keyout client.key \
       -out client.csr \
       -subj "/C=US/ST=CA/L=San Diego/O=Pivotal/OU=GemFire/CN=localhost"

# One way means that the server implicitly trusts clients so we are signing the client with a CA only known to the client.
openssl x509 -req -in client.csr -CA client-ca-certificate.pem -CAkey client-ca-key.pem -passin pass:changeit -CAcreateserial -out one-way-client.crt -days 500 -sha256

# Two way means that the server needs client CA in its trust store - so we are signing the client with the server CA.
openssl x509 -req -in client.csr -CA server-ca-certificate.pem -CAkey server-ca-key.pem -passin pass:changeit -CAcreateserial  -out two-way-client.crt -days 500 -sha256

cat client.key one-way-client.crt > one-way-client-keystore.pem

cat  one-way-client.crt client.key > one-way-reverse-client-keystore.pem

cat client.key two-way-client.crt > two-way-client-keystore.pem

cat client-ca-certificate.pem server-ca-certificate.pem > both-ca-truststore.pem

openssl crl2pkcs7 -nocrl -certfile both-ca-truststore.pem | openssl pkcs7 -print_certs -text -noout

cd "$SAVED" >&-