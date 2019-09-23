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

#include "leveldb/slice.h"

#include "base64.h"

namespace mcberepair {

inline std::string encode_key(const char *str, size_t size) {
	for(size_t i=0; i<size; ++i) {
		if(!std::isprint(static_cast<unsigned char>(str[i]))) {
			return base64_encode(reinterpret_cast<const unsigned char*>(str), size);
		}
	}
	return "plain:" + std::string{str, size};
}

inline std::string decode_key(const std::string &str) {
	if(str.find("plain:") == 0) {
		return str.substr(6);
	}
	return base64_decode(str);
}

inline bool is_chunk_key(const leveldb::Slice &key) {
	auto tag_test = [](char tag) {
		return ((45 <= tag && tag <= 58) || tag == 118);
	};

	if( key.size() == 9 || key.size() == 10) {
		return tag_test(key[8]);
	} else if( key.size() == 13 || key.size() == 14) {
		return tag_test(key[12]);
	}
	return false;
}

struct chunk_t {
	int dimension;
	int x;
	int z;
	char tag;
	char subtag; 
};

chunk_t parse_chunk_key(const leveldb::Slice &key) {
	chunk_t ret;
	assert(is_chunk_key(key));
	
	std::memcpy(&ret.x, key.data()+0, 4);
    std::memcpy(&ret.z, key.data()+4, 4);
    ret.dimension = 0;
    if(key.size() == 9) {
    	ret.tag = key[8];
    	ret.subtag = -1;
    } else if(key.size() == 10) {
    	ret.tag = key[8];
    	ret.subtag = key[9];
    } else if(key.size() == 13) {
    	std::memcpy(&ret.dimension, key.data()+8, 4);
    	ret.tag = key[12];
    	ret.subtag = -1;
    } else if(key.size() == 14) {
    	std::memcpy(&ret.dimension, key.data()+8, 4);
    	ret.tag = key[12];
    	ret.subtag = key[13];    	
    }

	return ret;	
}



} // namespace mcberepair

#endif
