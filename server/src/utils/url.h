
#pragma once

#include <string>

class Url
{
public:
    // url 编码
    static std::string encode(const std::string& str)
    {
        static auto hex_chars = "0123456789ABCDEF";

        std::string result;
        result.reserve(str.size()); // Minimum size of result

        for (auto& chr : str) {
            if (!((chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '-' || chr == '.' || chr == '_' || chr == '~'))
                result += std::string("%") + hex_chars[static_cast<unsigned char>(chr) >> 4] + hex_chars[static_cast<unsigned char>(chr) & 15];
            else
                result += chr;
        }

        return result;
    }

    // url 解码
    static std::string decode(const std::string& str)
    {
        std::string result;
        result.reserve(str.size());

        for (size_t i = 0; i < str.size(); ++i) {
            char ch = str[i];
            if (ch == '%') {
                constexpr char hex[] = "0123456789ABCDEF";

                if (++i == str.size()) {
                    result.push_back('?');
                    break;
                }

                int hi = (int)(std::find(hex, hex + 16, toupper(str[i])) - hex);

                if (++i == str.size()) {
                    result.push_back('?');
                    break;
                }

                int lo = (int)(std::find(hex, hex + 16, toupper(str[i])) - hex);

                if ((hi >= 16) || (lo >= 16)) {
                    result.push_back('?');
                    break;
                }

                result.push_back((char)((hi << 4) + lo));
            } else if (ch == '+')
                result.push_back(' ');
            else
                result.push_back(ch);
        }

        return result;
    }
}; // class Url