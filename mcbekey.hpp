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

#include "base64.h"

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

#endif
