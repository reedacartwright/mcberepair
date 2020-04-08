/*
# Copyright (c) 2020 Reed A. Cartwright <reed@cartwright.ht>
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
#include <cstdio>
#include <iostream>
#include <memory>
#include <cstring>

#include "db.hpp"

#include "mcbekey.hpp"

int repair_main(int argc, char *argv[]) {

    if(argc < 3 || strcmp("help", argv[1])==0) {
        printf("Usage: %s repair <minecraft_world_dir>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // construct path for Minecraft BE database
    std::string path = std::string(argv[2]) + "/db";

    leveldb::ZlibCompressorRaw zlib_raw;
    leveldb::ZlibCompressor zlib;

    leveldb::Options options;
    options.compressors[0] = &zlib_raw;
    options.compressors[1] = &zlib;

    leveldb::Status status;
    status = leveldb::RepairDB(path, options);

    if(!status.ok()) {
        fprintf(stderr, "ERROR: Repairing '%s' failed: %s\n", path.c_str(),
                status.ToString().c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
