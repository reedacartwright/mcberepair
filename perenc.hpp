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

#include <cassert>
#include <string>
#include <algorithm>

std::string percent_encode(const std::string &str);
void percent_decode(std::string *str);

inline
std::string percent_encode(const std::string &str) {
    auto is_graph = [](unsigned char c) { return std::isgraph(c); };

    // optimize for situation in which no encoding is needed
    auto it = std::find(str.begin(), str.end(), is_graph);
    if(it == str.end()) {
        return str;
    }
    char buffer[8];
    // setup return value
    std::string ret;
    ret.reserve(str.size());
    auto bit = str.begin();
    do {
        // Append sequences and encoded character
        std::snprintf(buffer, 8, "%%%02X", *it);
        ret.append(bit, it);
        ret.append(buffer);
        // Find next character to encode
        bit = ++it;
        it = std::find(it, str.end(), is_graph);
    } while( it != str.end());
    // Append tail
    ret.append(bit, str.end());

    return ret;
}

inline
int hex_decode(char x) {
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

inline
void percent_decode_core(std::string *str, size_t start) {
    assert(str != nullptr);
    assert(start < str->size());
    assert((*str)[start] == '%');
    auto p = str->begin() + start;
    auto q = p;
    int a, b;
    do {
        assert(*p == '%');
        if(++p == str->end()) {
            break;
        }
        a = hex_decode(*p);
        if(++p == str->end()) {
            break;
        }
        b = hex_decode(*p);
        if(a != -1 && b != -1) {
            *q++ = a * 16 + b;
        }
        for(++p; p != str->end(); ++p) {
            if(*p == '%') {
                break;
            }
            *q++ = *p;
        }
    } while(p != str->end());
    str->erase(q, str->end());
}

inline void percent_decode(std::string *str) {
    assert(str != nullptr);
    auto pos = str->find('%');
    if(pos != std::string::npos) {
        percent_decode_core(str, pos);
    }
}

#endif // MCBEREPAIR_PERCENC_HPP
