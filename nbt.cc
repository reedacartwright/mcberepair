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

#include "nbt.hpp"

char* read_payload(char *p, char *last, std::string_view name,
    std::vector<mcberepair::nbt_t> *v, mcberepair::nbt_type type);

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

    v->emplace_back(std::string_view{}, mcberepair::nbt_list_end_t{});

    return first;
}

inline
char* read_compound_payload(char *first, char *last, std::string_view name, std::vector<mcberepair::nbt_t> *v) {
    using nbt_type = mcberepair::nbt_type;

    v->emplace_back(name, mcberepair::nbt_compound_t{});

    char *p = first;
    while(p < last) {
        nbt_type type = nbt_type{*p};
        p += 1;
        if(type == nbt_type::END) {
            v->emplace_back(std::string_view{}, mcberepair::nbt_end_t{});
            return p;
        }
        // read the size of the name
        if(last-p < 2) {
            return nullptr; // malformed
        }
        uint16_t name_len;
        memcpy(&name_len, p, sizeof(name_len));
        p += 2;
        if(last-p < name_len) {
            return nullptr;
        }
        std::string_view child_name = {p,name_len};
        p = read_payload(p, last, child_name, v, type);
    }
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

bool parse_nbt(char *first, char *last) {
    assert(first != nullptr);
    assert(last != nullptr);
    assert(first <= last);

    using nbt_type = mcberepair::nbt_type;

    std::vector<mcberepair::nbt_t> nbt_data;

    if(first == last) {
        return true;
    }

    char *p = first;

    //std::stack<nbt_type> type_stack;
    //std::stack<unsigned int> count_stack; 

    int payload_index = 0;
    nbt_type type;
    while(p < last) {
        std::string_view name;
        if(payload_index == 0) {
            // read the type
            type = nbt_type{*p};
            p += 1;
            if(type == nbt_type::END) {
                name = {};
            } else {
                // read the size of the name
                if(last-p < 2) {
                    return false; // malformed
                }
                uint16_t name_len;
                memcpy(&name_len, p, sizeof(name_len));
                p += 2;
                if(last-p < name_len) {
                    return false;
                }
                name = {p,name_len};
            }
        } else {
            name = {};
        }
 
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
                p = read_simple_payload(p, last, name, &nbt_data, type);
                --payload_count;
                break;
            case nbt_type::COMPOUND:
                nbt_data.emplace_back(name, mcberepair::nbt_compound_t{});
                count_stack.push(payload_count);
                type_stack.push(type);
                payload_count = 0;
                break;
            case nbt_type::END:
                nbt_data.emplace_back(name, mcberepair::nbt_end_t{});
                payload_count = count_stack.top();
                type = type_stack.top();
                count_stack.pop();
                type_stack.pop();
                --payload_count;
                break;
            case nbt_type::LIST:
                p = read_payload<mcberepair::nbt_list_t>(p, last, name, &nbt_data);
                if(p == nullptr) {
                    return false;
                }
                count_stack.push(payload_count);
                type_stack.push(type);
                type = nbt_type{std::get<mcberepair::nbt_list_t>(nbt_data.back().payload).type};
                payload_count = std::get<mcberepair::nbt_list_t>(nbt_data.back().payload).size;
                break;
            default:
                return false;
        }
        if(p == nullptr) {
            return false;
        }
    }
    return true;
}
