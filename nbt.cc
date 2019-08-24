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

void read_nbt_impl(char ** const pfirst, char *last, mcberepair::nbt_type type,
    std::string_view name, std::vector<mcberepair::nbt_t> *v);

mcberepair::nbt_type read_type(char ** const pfirst, char *last) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);

    //std::cerr << "read_type\n";

    using nbt_type = mcberepair::nbt_type;

    char *p = *pfirst;

    if(p == last) {
        return nbt_type::END;
    }
    int8_t value;
    memcpy(&value, p, sizeof(value));

    *pfirst = p + sizeof(value);
    return nbt_type{value};
}

std::string_view read_name(char ** const pfirst, char *last) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);

    //std::cerr << "read_name\n";

    char *p = *pfirst;

    // read the size of the name
    uint16_t name_len;
    if(last-p < sizeof(name_len)) {
        return {}; // malformed
    }
    memcpy(&name_len, p, sizeof(name_len));
    p += sizeof(name_len);

    //std::cerr << "    " << name_len << "\n";

    // read the name
    if(last-p < name_len) {
        return {};
    }
    *pfirst = p + sizeof(char)*name_len;
    return {p,name_len};
}

template<typename T>
inline
std::enable_if_t<std::is_arithmetic_v<T>,mcberepair::nbt_t::payload_t>
read_payload_impl(char ** const pfirst, char *last) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);
    using payload_t = mcberepair::nbt_t::payload_t;

    //std::cerr << "read_payload_impl\n";

    char *p = *pfirst;
    if(last-p < sizeof(T)) {
        return {};
    }
    T value;
    memcpy(&value, p, sizeof(value));

    *pfirst += sizeof(T);

    return {value};
}

template<typename T>
inline
std::enable_if_t<std::is_member_pointer_v<decltype(&T::data)> &&
    std::is_member_pointer_v<decltype(&T::size)>, mcberepair::nbt_t::payload_t>
read_payload_impl(char ** const pfirst, char *last) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);
    using payload_t = mcberepair::nbt_t::payload_t;

    //std::cerr << "read_payload_impl_array\n";

    char *p = *pfirst;

    using array_size_t = decltype(T::size);

    if(last-p < sizeof(array_size_t)) {
        return {};
    }
    array_size_t array_size;
    memcpy(&array_size, p, sizeof(array_size_t));
    p += sizeof(array_size_t);
    size_t len = sizeof(std::remove_pointer_t<decltype(T::data)>)*array_size;
    if(last-p < len) {
        return {};
    }
    *pfirst = p + len;
    return T{array_size, reinterpret_cast<decltype(T::data)>(p)};
}

inline
void
read_simple_payload(char ** const pfirst, char *last, mcberepair::nbt_type type,
    std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    using nbt_type = mcberepair::nbt_type;
    mcberepair::nbt_t::payload_t payload;

    //std::cerr << "read_simple_payload\n";
    
    char *p = *pfirst;
    switch(type) {
    case nbt_type::BYTE:
        payload = read_payload_impl<int8_t>(pfirst, last);
        break;
    case nbt_type::SHORT:
        payload = read_payload_impl<int16_t>(pfirst, last);
        break;
    case nbt_type::INT:
        payload = read_payload_impl<int32_t>(pfirst, last);
        break;
    case nbt_type::LONG:
        payload = read_payload_impl<int64_t>(pfirst, last);
        break;
    case nbt_type::FLOAT:
        payload = read_payload_impl<float>(pfirst, last);
        break;
    case nbt_type::DOUBLE:
        payload = read_payload_impl<double>(pfirst, last);
        break;
    case nbt_type::BYTE_ARRAY:
        payload = read_payload_impl<mcberepair::nbt_byte_array_t>(pfirst, last);
        break;
    case nbt_type::STRING:
        payload = read_payload_impl<mcberepair::nbt_string_t>(pfirst, last);
        break;
    case nbt_type::INT_ARRAY:
        payload = read_payload_impl<mcberepair::nbt_int_array_t>(pfirst, last);
        break;
    case nbt_type::LONG_ARRAY:
        payload = read_payload_impl<mcberepair::nbt_long_array_t>(pfirst, last);
        break;
    default:
        return;
    }
    if(*pfirst != p) {
        v->emplace_back(name, payload);
    }
}


inline
void read_list_payload(char ** const pfirst, char *last, mcberepair::nbt_type type,
    std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);
    assert(v != nullptr);

    //std::cerr << "read_list_payload\n";

    using nbt_type = mcberepair::nbt_type;
    using nbt_t = mcberepair::nbt_t;
    using payload_t = mcberepair::nbt_t::payload_t;

    char *first = *pfirst;
    char *p = first;

    auto list_type = read_type(&p, last);
    if(list_type == nbt_type::END) {
        return; // Malformed
    }

    int32_t list_size;
    if(last-p < sizeof(list_size)) {
        return; // Malformed
    }
    memcpy(&list_size, p, sizeof(list_size));
    p += sizeof(list_size);

    v->emplace_back(name, mcberepair::nbt_list_t{list_size, list_type});
    for(int i=0;i<list_size;++i) {
        first = p;
        read_nbt_impl(&p, last, list_type, {}, v);
        if(first == p) {
            return; // Malformed
        }
    }
    *pfirst = p;
    v->emplace_back(std::string_view{}, payload_t{mcberepair::nbt_list_end_t{}});
}

inline
void read_compound_payload_impl(char ** const pfirst, char *last,
    std::vector<mcberepair::nbt_t> *v) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);
    assert(v != nullptr);

    //std::cerr << "read_compound_payload_impl\n";

    using nbt_type = mcberepair::nbt_type;

    char *first = *pfirst;
    while(first < last) {
        char *p = first;
        nbt_type type = read_type(&p, last);
        if(type == nbt_type::END) {
            *pfirst = p;
            return;
        }
        first = p;
        auto name = read_name(&p, last);
        if(p == first) {
            break;
        }
        first = p;

        read_nbt_impl(&p, last, type, name, v);
        if(p == first) {
            break;
        }
        first = p;
    }
}

inline
void read_compound_payload(char ** const pfirst, char *last, mcberepair::nbt_type type,
    std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    assert(pfirst != nullptr && *pfirst != nullptr);
    assert(last != nullptr);
    assert(*pfirst <= last);
    assert(v != nullptr);

    //std::cerr << "read_compound_payload\n";

    using nbt_type = mcberepair::nbt_type;

    v->emplace_back(name, mcberepair::nbt_compound_t{});

    char *first = *pfirst;
    read_compound_payload_impl(pfirst, last, v);
    if(first == *pfirst) {
        return; // malformed
    }
    v->emplace_back(std::string_view{}, mcberepair::nbt_end_t{});
}


inline
void read_nbt_impl(char ** const pfirst, char *last, mcberepair::nbt_type type,
    std::string_view name, std::vector<mcberepair::nbt_t> *v) {

    //std::cerr << "read_nbt_impl\n";

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
            read_simple_payload(pfirst, last, type, name, v);
            break;
        case nbt_type::LIST:
            read_list_payload(pfirst, last, type, name, v);
            break;
        case nbt_type::COMPOUND:
            read_compound_payload(pfirst, last, type, name, v);
            break;
        case nbt_type::END:
        default:
            //malformed
            assert(false);
            break;
    }
}

bool read_nbt(char *first, size_t length, std::vector<mcberepair::nbt_t> *nbt_data) {
    assert(first != nullptr);
    assert(nbt_data != nullptr);

    //std::cerr << "read_nbt\n";

    using nbt_type = mcberepair::nbt_type;

    if(length == 0) {
        return true;
    }

    char * last = first+length;

    read_compound_payload_impl(&first, last, nbt_data);

    return (first == last);
}

const char data[] = "\x09\x0b\0Hello World\x08\x02\0\0\0\x0b\0Hello World\x0b\0Hello_World";

#include "slurp.hpp"

int main(int argc, char *argv[]) {
    //std::string buffer(&data[0], sizeof(data));

    std::string buffer = mcberepair::slurp_string(std::cin);

    std::vector<mcberepair::nbt_t> result;
    bool res = read_nbt(buffer.data(),buffer.size(), &result);

    //std::cout << result.size() << "\n";
    for(auto &&a : result) {
        std::cout << a.name << " "  << a.payload.index() << "\n";
    }
    
    return res ? EXIT_SUCCESS : EXIT_FAILURE;
}