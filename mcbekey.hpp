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
