/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

// Include the Geode library.
#include <geode/GeodeCppCache.hpp>

// Use the "geode" namespace.
using namespace apache::geode::client;

using apache::geode::client::CacheableString;
using apache::geode::client::CacheFactory;
using apache::geode::client::RegionShortcut;

void print_usage() {
    printf("Usage: app <keystore> <truststore>\n");
    printf("Where <<path>> is the absolute path to the location of your SSL "
           "client keystore and truststore\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        exit(-1);
    }

    auto sslKeyStore = std::string(argv[1]);
    auto sslTrustStore = std::string(argv[2]);
    char hostbuffer[256];

    gethostname(hostbuffer, sizeof(hostbuffer));

    // Create a Geode Cache using CacheFactory. By default it will connect to
    // "localhost" at port 40404".

    CachePtr cachePtr = CacheFactory::createCacheFactory()
            ->set("log-level", "config")
            ->set("ssl-enabled", "true")
            ->set("statistic-sampling-enabled", "false")
            ->set("ssl-keystore", sslKeyStore.c_str())
            ->set("ssl-keystore-password", "changeit")
            ->set("ssl-truststore",sslTrustStore.c_str())
            ->addLocator(hostbuffer, 10334)
            ->set("security-username", "app")
            ->set("security-password", "1234567")
            ->set("security-client-auth-factory", "createGeodeUserPasswordAuth")
            ->set("security-client-auth-library", "libgeode_user_password_auth")
            ->create();

    LOGINFO("Created the Geode Cache");

    // Set Attributes for the region.
    RegionFactoryPtr regionFactory =
            cachePtr->createRegionFactory(PROXY);

    // Create exampleRegion.
    RegionPtr region = regionFactory->create("test");

    LOGINFO("Created the Region");


    std::string rtimmonsKey = "rtimmons";
    std::string rtimmonsValue = "Robert Timmons";
    std::string scharlesKey = "scharles";
    std::string scharlesValue = "Sylvia Charles";

    std::cout << "Storing id and username in the region" << std::endl;
    HashMapOfCacheable bulk;
    for(int i =0 ; i < 100; i++){
        for (int j = 0 ; j < 100 ; j++){
            bulk.insert(CacheableKey::create((rtimmonsKey + std::to_string(i) + " " + std::to_string(j)).c_str()), CacheableString::create(rtimmonsValue.c_str()));
            bulk.insert(CacheableKey::create((scharlesKey+ std::to_string(i) + " " + std::to_string(j)).c_str()), CacheableString::create(scharlesValue.c_str()));
        }
        region->putAll(bulk);
        bulk.clear();
    }
    VectorOfCacheableKey keys;
    region->serverKeys(keys);
    std::cout << "number of keys - " << keys.size() << std::endl;
}
