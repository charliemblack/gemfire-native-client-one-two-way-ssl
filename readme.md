# GemFire Native Client SSL Example

In this project I walk through all of the various ways to setup SSL with GemFire and GemFire Native Client.   This will include setting up one and two way SSL. Hopefully this helps someone understand SSL configuration.

Lets first establish what one and two way SSL is.   

One way is very common on the the internet.   Think of your browser whenever you connect to your bank for online bill pay you are using one way SSL.    Then think of the last time you visited a site and the ssl certificate expired or the site didn't have a certificate authority or CA that was in your browsers/system trusts store.   Your browser then cautioned you about that the server couldn't be trusted.

This is the same thing with GemFire one way SSL the client application doesn't need its certificate signed by a CA known by the server.   But the client should have the CA which signed the server certificate in its trust store.

Two way isn't as common as browsers and applications servers, but in the enterprise it is very common.   With two-way ssl Both the server and its clients need to signed by a CA that is known to each other.   If either client or server isn't in each others truststore then connection isn't allowed.

Which version your application uses depends on your own security posture.

# Sprinkle on Authentication

In this example I also put in some authentication.    I should point out that authentication has nothing todo with SSL.   However most of the time when someone turns on encryption they are also thinking about protecting via authentication.   

# Prep

For this application to work we need to download three requirements:

1. Pivotal GemFire
2. Pivotal GemFire Native Client version 9.x
2. A GemFire Native Client username and password library

* Download GemFire from pivotal network: https://network.pivotal.io/products/pivotal-gemfire
* Download GemFire Native Client from pivotal network: https://network.pivotal.io/products/pivotal-gemfire#/releases/405400
* A pre build GemFire Native Client User Name and Password Library can be found here: https://github.com/charliemblack/geode-cpp-user-password-auth/releases/tag/v0.1-ubuntu

If you would like to build the user name and password library your self feel free todo so.   Just look at the readme for that project.

Once you have downloaded and uncompressed GemFire Native Client library you need to either put the shared library into a well known library location OR set the `LD_LIBRARY_PATH` so the libraries can be found when running the `SampleApp`.

```
export LD_LIBRARY_PATH=~/dev/pivotal-gemfire-native-9.1.1-build.2-Linux-64bit/pivotal-gemfire-native/lib/
```

Then install the GemFire server by `uncompressing` it and adding the `bin` directory to your PATH.

```
export PATH=~/dev/pivotal-gemfire-9.8.4/bin:${PATH}
```

# Building

This sample application uses cmake - make sure you have downloaded and installed cmake and the proper compiler set for your os.

Since everyone's install location could be different I have made the location of the GemFire Native Client an environment variable - `GEMFIRE_NATIVE_HOME`.   Here is an example:

```
export GEMFIRE_NATIVE_HOME=/home/demo/dev/pivotal-gemfire-native-9.1.1-build.2-Linux-64bit/pivotal-gemfire-native
```

Build steps
```
cd <project>
mkdir build
cd build/
cmake ..
cmake --build .
```

# Running

Since the one or two way SSL aspect of this is all from the the lens of the server we need to start the server up running in the proper modes.   I have provided scripts to generate the SSL configuration and how to start the servers up in either mode.

## Generate the key and trust stores

Before we proceed with running anything we need some certificates.    This script will generate certificates based on the computer its run on and they are good for one year.  So run the `gen_certs.sh` script now.

```
cd <project>/scripts
./gen_certs.sh
```
Now we have all of the stores we need for all the various scenarios.

## One Way SSL

In this configuration the servers implicitly trust the clients.   The client's certificate authority (CA) doesn't need to be in the servers trust store.

Its the clients responsibility to verify that the server is "trusted".   The servers CA has to be in the client's trust store.

### Start The Servers

```
cd <project>/scripts
./startGemFireOneWaySSL.sh
```

If you would like to see the SSL configuration of a given server you can check out the ports that it is using and gather the metadata about its configuration via `openssl`.    Sample `openssl` command to dump information is:  `openssl s_client -connect server:40401`.

### Run the Client

Lets make two runs one that has the servers in the trust store another with out

#### With Server CA in the clients trust store - and the client key CA isn't in the servers truststore.
```
cd <project>
build/SampleApp certs/one-way-client-keystore.pem certs/server-ca-certificate.pem
```
**Success**

The client was able to connect using a key that was signed by a CA that was unknown to the server.  Also the server CA was in the clients trust store.

#### Without Servers CA in the clients trust store - and the client key CA isn't in the servers truststore.

```
cd <project>
build/SampleApp certs/one-way-client-keystore.pem certs/client-ca-certificate.pem
```
**Success**

Wait this isn't supposed to be a success.   **BUG - filed.**

### Shutdown the servers and clean up

```
cd <project>/scripts
./shutdownGemFire.sh
./clearGemFireData.sh
```

## Two Way SSL

Here the servers and clients need to be signed by a CA that is known by both the clients and the servers.    In another words the client and the servers need to have their CA in each others truststore.

### Start the servers

```
cd <project>/scripts
./startGemFireTwoWaySSL.sh
```

### Run the client

#### Have the client connect with cert signed by a CA that isn't in the servers trust store.
```
cd <project>
build/SampleApp certs/one-way-client-keystore.pem certs/both-ca-truststore.pem
```
**FAIL**
Which is a good thing.    The server rejected the connection because the CA that signed the server cert wasn't in the servers trust store.client

#### Have the client connect with a cert signed by a CA that is in the servers trust store.
```
cd <project>
build/SampleApp certs/two-way-client-keystore.pem certs/both-ca-truststore.pem
```
**Success**
The server allowed a connection from a client that had its certificate signed by a CA that is in the servers trust store.

#### Have the client connect with a valid cert but the servers cert isn't in the clients trust store.
```
cd <project>
build/SampleApp certs/two-way-client-keystore.pem certs/client-ca-truststore.pem
```
**Success**

Wait this isn't supposed to be a success.   BUG - filed.

### Shutdown the servers and clean up

```
cd <project>/scripts
./shutdownGemFire.sh
./clearGemFireData.sh
```