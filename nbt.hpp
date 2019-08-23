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

#ifndef MCBEREPAIR_NBT_HPP
#define MCBEREPAIR_NBT_HPP

#include <variant>
#include <string_view>

namespace mcberepair {

enum struct nbt_type: int {
    END = 0,
    BYTE = 1,
    SHORT = 2,
    INT = 3,
    LONG = 4,
    FLOAT = 5,
    DOUBLE = 6,
    BYTE_ARRAY = 7,
    STRING = 8,
    LIST = 9,
    COMPOUND = 10,
    INT_ARRAY = 11,
    LONG_ARRAY = 12
};

struct nbt_byte_array_t {
    int32_t size;
    int8_t *data;
};

struct nbt_int_array_t {
    int32_t size;
    int32_t *data;
};

struct nbt_long_array_t {
    int32_t size;
    int64_t *data;
};

struct nbt_string_t {
    int16_t size;
    char *data;
};

struct nbt_compound_t {

};

struct nbt_end_t {

};

struct nbt_list_t {
    int32_t size;
    int8_t type;
};

struct nbt_list_end_t {

};


struct nbt_t {
    std::string_view name;
    std::variant<int8_t,int16_t,int32_t,int64_t,
        float,double,
        nbt_byte_array_t, nbt_int_array_t, nbt_long_array_t,
        nbt_string_t,
        nbt_compound_t,
        nbt_end_t,
        nbt_list_t,
        nbt_list_end_t
        > payload;

};

}


#endif