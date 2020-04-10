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
#include <cstdio>
#include <memory>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "db.hpp"
#include "mcbekey.hpp"

int dumpkey_main(int argc, char* argv[]) {
    if(argc < 4 || strcmp("help", argv[1]) == 0) {
        printf("Usage: %s dumpkey <minecraft_world_dir> <key> > output.bin\n",
               argv[0]);
        return EXIT_FAILURE;
    }
    std::string value;

    // use RAII to close the db before dumping value
    {
        // construct path for Minecraft BE database
        std::string path = std::string(argv[2]) + "/db";

        // open the database
        mcberepair::DB db{path.c_str()};

        if(!db) {
            fprintf(stderr, "ERROR: Opening '%s' failed.\n", path.c_str());
            return EXIT_FAILURE;
        }

        // create a reusable memory space for decompression so it allocates less
        leveldb::ReadOptions readOptions;
        auto decompress_allocator =
            std::make_unique<leveldb::DecompressAllocator>();
        readOptions.decompress_allocator = decompress_allocator.get();
        readOptions.verify_checksums = true;

        std::string key;
        if(!mcberepair::decode_key(argv[3], &key)) {
            fprintf(stderr, "ERROR: key '%s' is malformed\n", argv[3]);
            return EXIT_FAILURE;
        }

        leveldb::Status status = db().Get(readOptions, key, &value);

        if(!status.ok()) {
            fprintf(stderr, "ERROR: Reading key '%s' failed --- %s\n", argv[3],
                    status.ToString().c_str());
            return EXIT_FAILURE;
        }
    }

#ifdef _WIN32
    fflush(stdout);
    _setmode(_fileno(stdout), O_BINARY);
#endif

    size_t ret = fwrite(value.data(), value.size(), 1, stdout);
    if(ret < 1) {
        return EXIT_FAILURE;  // LCOV_EXCL_LINE
    }
    return EXIT_SUCCESS;
}
