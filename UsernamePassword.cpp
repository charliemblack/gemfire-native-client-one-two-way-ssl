//
// Created by demo on 10/22/19.
//

#include "UsernamePassword.h"

#include <iostream>
#include <geode/CacheableString.hpp>

using apache::geode::client::AuthInitialize;
using apache::geode::client::Properties;
using apache::geode::client::CacheableString;

std::shared_ptr<Properties> UsernamePassword::getCredentials(
        const std::shared_ptr<Properties>& securityprops,
        const std::string& /*server*/) {
    std::cout << "ExampleAuthInitialize::GetCredentials called\n";

    auto properties = Properties::create();
    auto user = securityprops->find("security-username");
    auto password = securityprops->find("security-password");
    properties->insert("security-username", user->value().c_str());
    properties->insert("security-password", password->value().c_str());

    return properties;
}

void UsernamePassword::close() {
}

UsernamePassword::UsernamePassword() : AuthInitialize() {
}