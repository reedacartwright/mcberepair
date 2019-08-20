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

#ifndef SLURP_HPP
#define SLURP_HPP

#include <string>

namespace mcberepair {

template<typename T>
std::string slurp_string(T &in) {
    // create buffer
    std::string buffer(8192, '\0');
    std::size_t sz = 0;

    // read into buffer
    in.read(buffer.data()+sz,8192);
    // count how many values we read
    std::size_t g = in.gcount();
    sz += g;
    while(!in.eof()) {
        // allocate more space
        buffer.resize(buffer.size()+8192);
        // Keep reading
        in.read(buffer.data()+sz,8192);
        // count how many values we read
        g = in.gcount();
        sz += g;
    }
    // shrink buffer
    buffer.resize(sz);
    return buffer;
}

};

#endif
