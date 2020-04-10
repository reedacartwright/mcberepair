/*
# Copyright (c) 2019 Reed A. Cartwright <reed@cartwright.ht>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#ifndef MCBEREPAIR_PERENC_HPP
#define MCBEREPAIR_PERENC_HPP

#include <algorithm>
#include <cassert>
#include <string>
#include <string_view>

namespace mcberepair {

std::string percent_encode(std::string_view str);
bool percent_decode(std::string *str);

inline std::string percent_encode(std::string_view str) {
    auto is_notgraph = [](unsigned char c) {
        return std::isgraph(c) == 0 || c == '%' || c == '@';
    };

    // optimize for situation in which no encoding is needed
    auto it = std::find_if(str.begin(), str.end(), is_notgraph);
    if(it == str.end()) {
        return std::string{str};
    }
    char buffer[8];
    // setup return value
    std::string ret;
    ret.reserve(str.size());
    auto bit = str.begin();
    do {
        // Append sequences and encoded character
        unsigned char c = *it;
        std::snprintf(buffer, 8, "%%%02hhX", c);
        ret.append(bit, it);
        ret.append(buffer);
        // Find next character to encode
        bit = ++it;
        it = std::find_if(it, str.end(), is_notgraph);
    } while(it != str.end());
    // Append tail
    ret.append(bit, str.end());

    return ret;
}

inline int hex_decode(char x) {
    if('0' <= x && x <= '9') {
        return x - '0';
    }
    if('A' <= x && x <= 'F') {
        return x - 'A' + 10;
    }
    if('a' <= x && x <= 'f') {
        return x - 'a' + 10;
    }
    return -1;
}

inline bool percent_decode_core(std::string *str, size_t start) {
    assert(str != nullptr);
    assert(start < str->size());
    assert((*str)[start] == '%');
    auto p = str->begin() + start;
    auto q = p;
    do {
        assert(*p == '%');
        if(++p == str->end()) {
            return false;
        }
        int a = hex_decode(*p);
        if(++p == str->end()) {
            return false;
        }
        int b = hex_decode(*p);
        if(a == -1 || b == -1) {
            return false;
        }
        *q++ = a * 16 + b;
        for(++p; p != str->end(); ++p) {
            if(*p == '%') {
                break;
            }
            *q++ = *p;
        }
    } while(p != str->end());
    str->erase(q, str->end());
    return true;
}

inline bool percent_decode(std::string *str) {
    assert(str != nullptr);
    auto pos = str->find('%');
    if(pos != std::string::npos) {
        return percent_decode_core(str, pos);
    }
    return true;
}

}  // namespace mcberepair

#endif  // MCBEREPAIR_PERCENC_HPP
