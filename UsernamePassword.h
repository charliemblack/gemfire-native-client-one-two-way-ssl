//
// Created by demo on 10/22/19.
//

#ifndef SAMPLEAPP_USERNAMEPASSWORD_H
#define SAMPLEAPP_USERNAMEPASSWORD_H

#include <memory>
#include <string>

#include <geode/AuthInitialize.hpp>
#include <geode/Properties.hpp>
#include <geode/SystemProperties.hpp>

class UsernamePassword : public apache::geode::client::AuthInitialize {
    std::shared_ptr<apache::geode::client::Properties> getCredentials(
            const std::shared_ptr<apache::geode::client::Properties>& securityprops,
            const std::string& server) override;

    void close() override;

public:
    UsernamePassword();
    ~UsernamePassword() noexcept = default;
};


#endif //SAMPLEAPP_USERNAMEPASSWORD_H
