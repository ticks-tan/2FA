#include "otpauth.h"

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>

#include "base32.h"
#include "url.h"

namespace OTPAuth
{

    // 生成 OTP 值
    std::string generate_otp(const std::string& key, OTPAuth::HashAlgorithm algorithm, uint64_t counter, unsigned digits)
    {
        // 将计数器转换为大端序字节数组
        std::array<unsigned char, 8> counter_bytes{};
        for (int i = 7; i >= 0; --i) {
            counter_bytes[i] = counter & 0xff;
            counter >>= 8;
        }

        // 计算 HMAC 值
        unsigned int hmac_len = 0;
        unsigned char hmac_result[EVP_MAX_MD_SIZE];
        HMAC(get_evp_md(algorithm), key.c_str(), key.size(), counter_bytes.data(),
             counter_bytes.size(), hmac_result, &hmac_len);

        // 获取动态截断值
        uint64_t offset = hmac_result[hmac_len - 1] & 0x0f;
        uint64_t truncated_hash = ((hmac_result[offset] & 0x7f) << 24) | ((hmac_result[offset + 1] & 0xff) << 16) | ((hmac_result[offset + 2] & 0xff) << 8) | (hmac_result[offset + 3] & 0xff);

        // 计算 OTP 值
        uint64_t otp_value =
            truncated_hash % static_cast<uint64_t>(std::pow(10, digits));

        // 格式化 OTP 值
        std::stringstream ss;
        ss << std::setw(digits) << std::setfill('0') << otp_value;
        return ss.str();
    }

    /* ---------------------------------------------------- */
    bool TOTP::check() const
    {
        if (_config.label.empty()) {
            return false;
        }
        if (_config.secret.empty()) {
            return false;
        }
        if (_config.digits < 6 || _config.digits > 32) {
            return false;
        }
        if (_config.period == 0 || _config.period > 60) {
            return false;
        }
        return true;
    }

     std::string TOTP::toUrl() const
    {
        std::stringstream ss;
        ss << "otpauth://hotp/";
        if (!_config.issuer.empty()) {
            ss << _config.label
               << "?issuer="
               << _config.issuer
               << '&';
        } else {
            ss << _config.label
               << '?';
        }
        ss << "secret=" << Url::encode(_config.secret)
           << "&algorithm=" << Url::encode(get_hash_aligorithm_name(_config.algorithm))
           << "&digits=" << Url::encode(std::to_string(_config.digits))
           << "&period=" << Url::encode(std::to_string(_config.period));
        return ss.str();
    }

    std::string TOTP::generator(unsigned long long time_step) const
    {
        if (!this->check()) {
            // 配置错误
            throw std::runtime_error("Invalid configuration");
        }
        if (time_step == 0) {
            time_step = static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::seconds>(
                                                                 std::chrono::system_clock::now().time_since_epoch())
                                                                 .count()
                                                             / _config.period);
        }
        std::string decode_secret = Base32::decode(_config.secret);
        return generate_otp(decode_secret, _config.algorithm, time_step, _config.digits);
    }

    bool TOTP::validate(const std::string& token, unsigned window) const
    {
        return HOTP(HOTP::Config{
            .algorithm = _config.algorithm,
            .digits = _config.digits,
            .counter = _config.period,
            .secret = _config.secret
        }).validate(token, window);
    }

    /* ----------------------------------------------------- */
    bool HOTP::check() const
    {
        if (_config.label.empty()) {
            return false;
        }
        if (_config.secret.empty()) {
            return false;
        }
        if (_config.digits < 6 || _config.digits > 32) {
            return false;
        }
        return true;
    }

    std::string HOTP::toUrl() const
    {
        std::stringstream ss;
        ss << "otpauth://hotp/";
        if (!_config.issuer.empty()) {
            ss << _config.label
               << "?issuer="
               << _config.issuer
               << '&';
        } else {
            ss << _config.label
               << '?';
        }
        ss << "secret=" << Url::encode(_config.secret)
           << "&algorithm=" << Url::encode(get_hash_aligorithm_name(_config.algorithm))
           << "&digits=" << Url::encode(std::to_string(_config.digits))
           << "&counter=" << Url::encode(std::to_string(_config.counter));
        return ss.str();
    }

    std::string HOTP::generator() const
    {
        if (!this->check()) {
            // 配置错误
            throw std::runtime_error("Invalid configuration");
        }
        std::string decode_secret = Base32::decode(_config.secret);
        return generate_otp(decode_secret, _config.algorithm, _config.counter, _config.digits);
    }

    bool HOTP::validate(const std::string& token, unsigned window) const
    {
        if (token.size() != _config.digits) {
            return false;
        }
        std::optional<bool> delta;
        auto checkFunc = [&token, &delta, this](unsigned long long) {
            const std::string generated_token = HOTP(Config{
                                                         .algorithm = this->_config.algorithm,
                                                         .digits = this->_config.digits,
                                                         .counter = this->_config.counter,
                                                         .secret = this->_config.secret,
                                                     })
                                                    .generator();
            if (generated_token == token) {
                delta = true;
            }
        };
        checkFunc(this->_config.counter);
        for (int i = 1; i <= window && !delta.has_value(); ++i) {
            checkFunc(this->_config.counter - i);
            if (delta.has_value()) {
                break;
            }
            checkFunc(this->_config.counter + i);
            if (delta.has_value()) {
                break;
            }
        }
        return delta.has_value();
    }

} // namespace OTPAuth