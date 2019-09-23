#ifndef PERENC_H
#define PERENC_H

#include <cassert>
#include <string>

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

#endif