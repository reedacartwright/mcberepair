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

#include "db.hpp"
#include "mcbekey.hpp"

int copyall_main(int argc, char *argv[]) {
    if(argc < 4 || strcmp("help", argv[1]) == 0) {
        printf(
            "Usage: %s copyall <source_minecraft_world_dir> "
            "<dest_minecraft_world_dir>\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    // construct path for Minecraft BE database
    std::string path = std::string(argv[2]) + "/db";
    std::string copy_path = std::string(argv[3]) + "/db";

    // open the input database
    mcberepair::DB db{path.c_str()};

    if(!db) {
        fprintf(stderr, "ERROR: Opening '%s' failed.\n", path.c_str());
        return EXIT_FAILURE;
    }

    // open the destination database
    mcberepair::DB copy_db{copy_path.c_str(), true, true};

    if(!copy_db) {
        fprintf(stderr, "ERROR: Opening '%s' failed.\n", copy_path.c_str());
        return EXIT_FAILURE;
    }

    // create a reusable memory space for decompression so it allocates less
    leveldb::ReadOptions readOptions;
    leveldb::DecompressAllocator decompress_allocator;
    readOptions.decompress_allocator = &decompress_allocator;
    readOptions.verify_checksums = true;
    readOptions.fill_cache = false;

    // create an iterator for the database
    auto it = std::unique_ptr<leveldb::Iterator>{db().NewIterator(readOptions)};

    leveldb::Status status;

    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        auto key = it->key();
        auto value = it->value();
        status = copy_db().Put({}, key, value);
        if(!status.ok()) {
            // LCOV_EXCL_START
            fprintf(stderr, "ERROR: copying key '%s' failed: %s\n",
                    key.ToString().c_str(), status.ToString().c_str());
            return EXIT_FAILURE;
            // LCOV_EXCL_STOP
        }
    }
    copy_db().CompactRange(nullptr, nullptr);

    if(!status.ok()) {
        // LCOV_EXCL_START
        fprintf(stderr, "ERROR: copying '%s' to '%s' failed: %s\n",
                path.c_str(), copy_path.c_str(), status.ToString().c_str());
        return EXIT_FAILURE;
        // LCOV_EXCL_STOP
    }
    return EXIT_SUCCESS;
}
