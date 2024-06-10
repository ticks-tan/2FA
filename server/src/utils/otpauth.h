
#pragma once

#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <string>

namespace OTPAuth
{

    // 支持的哈希算法
    enum class HashAlgorithm
    {
        SHA1,
        SHA256,
        SHA512,
        MD5
    };
    // 获取 OpenSSL EVP_MD 对象
    inline const EVP_MD* get_evp_md(HashAlgorithm algorithm)
    {
        switch (algorithm) {
        case HashAlgorithm::SHA1:
            return EVP_sha1();
        case HashAlgorithm::SHA256:
            return EVP_sha256();
        case HashAlgorithm::SHA512:
            return EVP_sha512();
        case HashAlgorithm::MD5:
            return EVP_md5();
        default:
            return EVP_sha1();
        }
    }
    // 获取 Hash 函数名称
    inline const char* get_hash_aligorithm_name(HashAlgorithm algorithm)
    {
        switch (algorithm) {
        case HashAlgorithm::SHA1:
            return "SHA1";
        case HashAlgorithm::SHA256:
            return "SHA256";
        case HashAlgorithm::SHA512:
            return "SHA256";
        case HashAlgorithm::MD5:
            return "MD5";
        default:
            return "SHA1";
        }
    }

    // HOTP 对象
    class HOTP
    {
    public:
        struct Config
        {
            std::string issuer{};
            std::string label{};
            HashAlgorithm algorithm{HashAlgorithm::SHA1};
            unsigned digits{6};
            unsigned long counter{0};
            std::string secret{};
        }; // struct Config

    public:
        explicit HOTP()
        {}
        explicit HOTP(const Config& cfg)
            : _config(cfg)
        {}
        ~HOTP() = default;

        Config& config() { return _config; }
        // 转换为 Google Authenticator URL
        std::string toUrl() const;
        // 生成 token
        std::string generator() const;
        // 验证 token
        bool validate(const std::string& token, unsigned window = 1) const;

    private:
        // 检查配置是否合格
        bool check() const;

    private:
        Config _config;

    }; // class HOTP

    // 生成 OTP 值
    std::string generate_otp(const std::string& key, uint64_t counter);

    class TOTP
    {
    public:
        struct Config
        {
            std::string issuer{};
            std::string label{};
            HashAlgorithm algorithm{HashAlgorithm::SHA1};
            unsigned digits{6};
            unsigned period{30};
            std::string secret{};
        }; // struct Config

    public:
        explicit TOTP()
        {}
        explicit TOTP(const Config& cfg)
            : _config(cfg)
        {}
        ~TOTP() = default;

        Config& config() { return _config; }
        // 转换为 Google Authenticator URL
        std::string toUrl() const;
        // 生成代码
        std::string generator(unsigned long long time_step = 0) const;
        // 验证 token
        bool validate(const std::string& token, unsigned window = 1) const;

    private:
        // 检查配置是否合格
        bool check() const;

    private:
        Config _config;
    }; // class TOTP

} // namespace OTPAuth