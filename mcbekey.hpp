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

#ifndef MCBEKEY_HPP
#define MCBEKEY_HPP

#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "leveldb/slice.h"
#include "perenc.hpp"

namespace mcberepair {

inline bool is_chunk_key(std::string_view key) {
    auto tag_test = [](char tag) {
        return ((45 <= tag && tag <= 64) || tag == 118);
    };

    if(key.size() == 9 || key.size() == 10) {
        return tag_test(key[8]);
    } else if(key.size() == 13 || key.size() == 14) {
        return tag_test(key[12]);
    }
    return false;
}

// Chunk information
// See https://minecraft.gamepedia.com/Bedrock_Edition_level_format
struct chunk_t {
    int dimension;
    int x;
    int z;
    char tag;
    char subtag;
};

inline chunk_t parse_chunk_key(std::string_view key) {
    chunk_t ret;
    assert(is_chunk_key(key));

    std::memcpy(&ret.x, key.data() + 0, 4);
    std::memcpy(&ret.z, key.data() + 4, 4);
    ret.dimension = 0;
    if(key.size() == 9) {
        ret.tag = key[8];
        ret.subtag = -1;
    } else if(key.size() == 10) {
        ret.tag = key[8];
        ret.subtag = key[9];
    } else if(key.size() == 13) {
        std::memcpy(&ret.dimension, key.data() + 8, 4);
        ret.tag = key[12];
        ret.subtag = -1;
    } else if(key.size() == 14) {
        std::memcpy(&ret.dimension, key.data() + 8, 4);
        ret.tag = key[12];
        ret.subtag = key[13];
    }

    return ret;
}

inline void create_chunk_key(chunk_t chunk, std::string *out) {
    assert(out != nullptr);
    char buffer[16];
    std::memcpy(buffer + 0, &chunk.x, 4);
    std::memcpy(buffer + 4, &chunk.z, 4);
    int off = 8;
    if(chunk.dimension != 0) {
        std::memcpy(buffer + 8, &chunk.dimension, 4);
        off += 4;
    }
    buffer[off] = chunk.tag;
    if(chunk.subtag != -1) {
        off += 1;
        buffer[off] = chunk.subtag;
    }
    out->assign(buffer, buffer + off + 1);
}

inline std::string encode_key(std::string_view key) {
    if(!is_chunk_key(key)) {
        return percent_encode(key);
    }
    auto chunk = parse_chunk_key(key);
    std::stringstream str;
    str << "@" << chunk.x << ":" << chunk.z << ":" << chunk.dimension << ":"
        << static_cast<unsigned int>(chunk.tag);

    if(chunk.subtag != -1) {
        str << "-" << static_cast<unsigned int>(chunk.subtag);
    }

    return str.str();
}

inline bool decode_key(std::string_view key, std::string *out) {
    assert(out != nullptr);
    if(key.empty()) {
        out->assign(key);
        return true;
    }
    if(key[0] != '@') {
        out->assign(key);
        return mcberepair::percent_decode(out);
    }
    std::string buf{key.substr(1)};
    std::stringstream str(buf);
    chunk_t chunk;
    if(!(str >> chunk.x)) {
        return false;
    }
    if(str.peek() == ':') {
        str.ignore();
    }
    if(!(str >> chunk.z)) {
        return false;
    }
    if(str.peek() == ':') {
        str.ignore();
    }
    if(!(str >> chunk.dimension)) {
        return false;
    }
    unsigned int tag;
    if(str.peek() == ':') {
        str.ignore();
    }
    if(!(str >> tag)) {
        return false;
    }
    chunk.tag = tag;
    chunk.subtag = -1;
    if(str.peek() == '-') {
        str.ignore();
        if(!(str >> tag)) {
            return false;
        }
        chunk.subtag = tag;
    }
    if(!str.eof()) {
        return false;
    }
    create_chunk_key(chunk, out);
    return true;
}

}  // namespace mcberepair

#endif
