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
#include <geode/Cache.hpp>
#include <geode/CacheFactory.hpp>
#include <geode/PoolManager.hpp>
#include <geode/RegionFactory.hpp>
#include <geode/RegionShortcut.hpp>
#include "UsernamePassword.h"

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


    auto cachePtr =
            CacheFactory()
            .set("log-level", "config")
            .set("ssl-enabled", "true")
            .set("statistic-sampling-enabled", "false")
            .set("ssl-keystore", sslKeyStore)
            .set("ssl-keystore-password", "changeit")
            .set("ssl-truststore",sslTrustStore)
            .set("security-username", "app")
            .set("security-password", "1234567")
            .setAuthInitialize(std::unique_ptr<UsernamePassword>(new UsernamePassword()))
            .create();


    cachePtr.getPoolManager().createFactory()
            .addLocator(hostbuffer, 10334)
            .create("pool");

    auto region = cachePtr.createRegionFactory(RegionShortcut::PROXY)
            .setPoolName("pool")
            .create("test");




    std::string rtimmonsKey = "rtimmons";
    std::string rtimmonsValue = "Robert Timmons";
    std::string scharlesKey = "scharles";
    std::string scharlesValue = "Sylvia Charles";

    std::cout << "Storing id and username in the region" << std::endl;

    for(int i =0 ; i < 10; i++){
        for (int j = 0 ; j < 10 ; j++){
            region->put(
                    CacheableKey::create((rtimmonsKey + std::to_string(i) + " " + std::to_string(j))),
                    CacheableString::create(rtimmonsValue));
            region->put(
                    CacheableKey::create((scharlesKey + std::to_string(i) + " " + std::to_string(j))),
                    CacheableString::create(scharlesValue));
        }
    }
    auto serverKeys = region->serverKeys();
    std::cout << "number of keys - " << serverKeys.size() << std::endl;
}
