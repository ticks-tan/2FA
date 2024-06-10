
#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

// Base32 编码表
const char _gEncodingTable[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Base32 解码表
const std::map<char, uint32_t> _gDecodingTable{
    {'A', 0},
    {'B', 1},
    {'C', 2},
    {'D', 3},
    {'E', 4},
    {'F', 5},
    {'G', 6},
    {'H', 7},
    {'I', 8},
    {'J', 9},
    {'K', 10},
    {'L', 11},
    {'M', 12},
    {'N', 13},
    {'O', 14},
    {'P', 15},
    {'Q', 16},
    {'R', 17},
    {'S', 18},
    {'T', 19},
    {'U', 20},
    {'V', 21},
    {'W', 22},
    {'X', 23},
    {'Y', 24},
    {'Z', 25},
    {'2', 26},
    {'3', 27},
    {'4', 28},
    {'5', 29},
    {'6', 30},
    {'7', 31},
};

struct Base32
{
    // Base32 编码
    static std::string encode(const std::string& text, bool omit_padding = false)
    {
        std::vector<uint8_t> data{text.begin(), text.end()};

        std::ostringstream output;
        size_t bits = 0;
        uint32_t buffer = 0;
        for (auto datum: data) {
            buffer <<= 8;
            buffer += (uint32_t)datum;
            bits += 8;
            while (bits >= 5) {
                output << _gEncodingTable[(buffer >> (bits - 5)) & 0x3f];
                buffer &= ~(0x1f << (bits - 5));
                bits -= 5;
            }
        }
        if ((data.size() % 5) == 1) {
            buffer <<= 2;
            output << _gEncodingTable[buffer & 0x1f];
            if (!omit_padding) {
                output << "======";
            }
        } else if ((data.size() % 5) == 2) {
            buffer <<= 4;
            output << _gEncodingTable[buffer & 0x1f];
            if (!omit_padding) {
                output << "====";
            }
        } else if ((data.size() % 5) == 3) {
            buffer <<= 1;
            output << _gEncodingTable[buffer & 0x1f];
            if (!omit_padding) {
                output << "===";
            }
        } else if ((data.size() % 5) == 4) {
            buffer <<= 3;
            output << _gEncodingTable[buffer & 0x1f];
            if (!omit_padding) {
                output << '=';
            }
        }
        return output.str();
    }

    // Base32 解码
    static std::string decode(const std::string& text)
    {
        std::vector<uint8_t> data{text.begin(), text.end()};
        std::ostringstream output;
        uint32_t buffer = 0;
        size_t bits = 0;
        for (auto datum : data) {
            const auto entry = _gDecodingTable.find(datum);
            uint32_t group = 0;
            if (entry != _gDecodingTable.end()) {
                group = entry->second;
            }
            buffer <<= 5;
            bits += 5;
            buffer += group;
            if (bits >= 8) {
                if (datum != '=') {
                    output << (char)(buffer >> (bits - 8));
                }
                buffer &= ~(0xff << (bits - 8));
                bits -= 8;
            }
        }
        return output.str();
    }

}; // struct Base32