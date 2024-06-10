#include <iostream>
#include <chrono>
#include <thread>
#include "utils/otpauth.h"

int main()
{
    auto totp = OTPAuth::TOTP(OTPAuth::TOTP::Config{
        .label = "Test",
        .algorithm = OTPAuth::HashAlgorithm::MD5,
        .digits = 6,
        .period = 30,
        .secret = "PJ4GG5TCNZWS4MJUG4YDSNRTGI"
    });

    for (int i = 0; i < 300; ++i) {
        std::cout << "\x1b[2J\x1b[0;0ftotp code: " << totp.generator() << "\n";
        std::cout << "google authrizer url: " << totp.toUrl() << "\n";
        std::cout << "remaining seconds: " << (30 - std::time(nullptr) % 30) << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}