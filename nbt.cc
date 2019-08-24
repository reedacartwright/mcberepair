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

#include <cassert>
#include <vector>
#include <cstring>
#include <stack>
#include <string>
#include <iostream>

#include "nbt.hpp"

char* read_payload(char *p, char *last, std::string_view name,
    std::vector<mcberepair::nbt_t> *v, mcberepair::nbt_type type);

char* read_name(char *p, char *last, std::string_view *name) {
        assert(p != nullptr);
        assert(last != nullptr);
        assert(p <= last);
        assert(name != nullptr);
        // read the size of the name
        if(last-p < 2) {
            return nullptr; // malformed
        }
        uint16_t name_len;
        memcpy(&name_len, p, sizeof(name_len));
        p += 2;
        // read the name
        if(last-p < name_len) {
            return nullptr;
        }
        *name = {p,name_len};
        p += sizeof(char)*name_len;
        return p;
}

template<typename T>
inline
char* read_payload_impl(char *first, char *last, std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    assert(first != nullptr);
    assert(last != nullptr);
    assert(first <= last);
    assert(v != nullptr);

    if(last-first < sizeof(T)) {
        return nullptr;
    }
    T val;
    memcpy(&val, first, sizeof(T));
    v->emplace_back(name, val);

    return first+sizeof(T);
}

template<typename T>
inline
std::enable_if_t<std::is_member_pointer_v<&T::data> && std::is_member_pointer_v<&T::size>, char*>
read_payload_impl(char *first, char *last, std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    assert(first != nullptr);
    assert(last != nullptr);
    assert(first <= last);
    assert(v != nullptr);

    using array_size_t = decltype(T::size);

    if(last-first < sizeof(array_size_t)) {
        return nullptr;
    }
    array_size_t array_size;
    memcpy(&array_size, first, sizeof(array_size_t));
    first += sizeof(int32_t);
    if(last-first < sizeof(std::remove_pointer_t<decltype(T::data)>)*array_size) {
        return nullptr;
    }
    v->emplace_back(name, T{array_size, reinterpret_cast<decltype(T::data)>(first)});

    return first + sizeof(T)*array_size;
}

inline
char* read_list_payload(char *first, char *last, std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    assert(first != nullptr);
    assert(last != nullptr);
    assert(first <= last);
    assert(v != nullptr);

    using nbt_type = mcberepair::nbt_type;
    using nbt_t = mcberepair::nbt_t;
    using payload_t = mcberepair::nbt_t::payload_t;

    if(last-first < sizeof(int8_t)) {
        return nullptr;
    }
    int8_t list_type;
    memcpy(&list_type, first, sizeof(int8_t));

    first += sizeof(int8_t);

    if(last-first < sizeof(int32_t)) {
        return nullptr;
    }
    int32_t list_size;
    memcpy(&list_size, first, sizeof(int32_t));

    first += sizeof(int32_t);

    v->emplace_back(name, mcberepair::nbt_list_t{list_size, list_type});
    for(int i=0;i<list_size && first != nullptr;++i) {
        first = read_payload(first, last, {}, v, mcberepair::nbt_type{list_type});
    }

    v->emplace_back(std::string_view{}, payload_t{mcberepair::nbt_list_end_t{}});

    return first;
}

inline
char* read_compound_payload(char *first, char *last, std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    using nbt_type = mcberepair::nbt_type;

    v->emplace_back(name, mcberepair::nbt_compound_t{});

    char *p = first;
    while(p < last && p != nullptr) {
        nbt_type type = nbt_type{*p};
        p += 1;
        if(type == nbt_type::END) {
            v->emplace_back(std::string_view{}, mcberepair::nbt_end_t{});
            return p;
        }
        std::string_view child_name;
        p = read_name(p, last, &child_name);
        if(p == nullptr) {
            break;
        }
        p = read_payload(p, last, child_name, v, type);
    }
    // data ended without finding an end tag
    return nullptr;
}

inline
char* read_simple_payload(char *p, char *last, std::string_view name,
    std::vector<mcberepair::nbt_t> *v, mcberepair::nbt_type type) {
    using nbt_type = mcberepair::nbt_type;
    switch(type) {
    case nbt_type::BYTE:
        p = read_payload_impl<int8_t>(p, last, name, v);
        break;
    case nbt_type::SHORT:
        p = read_payload_impl<int16_t>(p, last, name, v);
        break;
    case nbt_type::INT:
        p = read_payload_impl<int32_t>(p, last, name, v);
        break;
    case nbt_type::LONG:
        p = read_payload_impl<int64_t>(p, last, name, v);
        break;
    case nbt_type::FLOAT:
        p = read_payload_impl<float>(p, last, name, v);
        break;
    case nbt_type::DOUBLE:
        p = read_payload_impl<double>(p, last, name, v);
        break;
    case nbt_type::BYTE_ARRAY:
        p = read_payload_impl<mcberepair::nbt_byte_array_t>(p, last, name, v);
        break;
    case nbt_type::STRING:
        p = read_payload_impl<mcberepair::nbt_string_t>(p, last, name, v);
        break;
    case nbt_type::INT_ARRAY:
        p = read_payload_impl<mcberepair::nbt_int_array_t>(p, last, name, v);
        break;
    case nbt_type::LONG_ARRAY:
        p = read_payload_impl<mcberepair::nbt_long_array_t>(p, last, name, v);
        break;
    default:
        return nullptr;
    }
    return p;
}

inline
char* read_payload(char *p, char *last, std::string_view name,
    std::vector<mcberepair::nbt_t> *v, mcberepair::nbt_type type) {

    using nbt_type = mcberepair::nbt_type;

    switch(type) {
        case nbt_type::BYTE:
        case nbt_type::SHORT:
        case nbt_type::INT:
        case nbt_type::LONG:
        case nbt_type::FLOAT:
        case nbt_type::DOUBLE:
        case nbt_type::BYTE_ARRAY:
        case nbt_type::STRING:
        case nbt_type::INT_ARRAY:
        case nbt_type::LONG_ARRAY:
            return read_simple_payload(p, last, name, v, type);
        case nbt_type::LIST:
            return read_list_payload(p, last, name, v);
        case nbt_type::COMPOUND:
            return read_compound_payload(p, last, name, v);
        case nbt_type::END:
        default:
            //malformed
            break;
    }
    return nullptr;
}

bool read_nbt(char *first, char *last, std::vector<mcberepair::nbt_t> *nbt_data) {
    assert(first != nullptr);
    assert(last != nullptr);
    assert(first <= last);
    assert(nbt_data != nullptr);

    using nbt_type = mcberepair::nbt_type;


    if(first == last) {
        return true;
    }

    char *p = first;
    while(p < last && p != nullptr) {
        nbt_type type = nbt_type{*p};
        p += 1;

        std::string_view child_name;
        p = read_name(p, last, &child_name);

        //std::cerr << (int)type << " " << child_name << "\n";

        if(p == nullptr) {
            return false;
        }
        p = read_payload(p, last, child_name, nbt_data, type);
    }
    return (p != nullptr);
}

const char data[] = "\x09\x0b\0Hello World\x08\x02\0\0\0\x0b\0Hello World\x0b\0Hello_World";

#include "slurp.hpp"

int main(int argc, char *argv[]) {
    //std::string buffer(&data[0], sizeof(data));


    std::string buffer = mcberepair::slurp_string(std::cin);


    std::vector<mcberepair::nbt_t> result;
    bool res = read_nbt(buffer.data(),buffer.data()+buffer.size(), &result);

    std::cout << result.size() << "\n";
    for(auto &&a : result) {
        std::cout << a.name << "\n";
    }
    
    return res ? EXIT_SUCCESS : EXIT_FAILURE;
}