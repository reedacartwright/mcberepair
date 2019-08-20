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
#include <iostream>
#include <memory>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "leveldb/decompress_allocator.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/zlib_compressor.h"

#include "mcbekey.hpp"

int main(int argc, char* argv[]) {
    if(argc < 3) {
        printf("Usage: %s <minecraft_world_dir> <key> < input.bin\n", argv[0]);
        return EXIT_FAILURE;
    }

    class NullLogger : public leveldb::Logger {
       public:
        void Logv(const char*, va_list) override {}
    };

    leveldb::Options options;

    // create a bloom filter to quickly tell if a key is in the database or not
    auto filter_policy = std::unique_ptr<const leveldb::FilterPolicy>{leveldb::NewBloomFilterPolicy(10)};
    options.filter_policy = filter_policy.get();

    // create a 40 mb cache (we use this on ~1gb devices)
    auto block_cache = std::unique_ptr<leveldb::Cache>{ leveldb::NewLRUCache(40 * 1024 * 1024) };
    options.block_cache = block_cache.get();

    // create a 4mb write buffer, to improve compression and touch the disk less
    options.write_buffer_size = 4 * 1024 * 1024;

    // disable internal logging. The default logger will still print out things
    // to a file
    auto logger = std::make_unique<NullLogger>();
    options.info_log = logger.get();

    // use the new raw-zip compressor to write (and read)
    auto zlib_raw_compressor = std::make_unique<leveldb::ZlibCompressorRaw>(-1);
    options.compressors[0] = zlib_raw_compressor.get();

    // also setup the old, slower compressor for backwards compatibility. This
    // will only be used to read old compressed blocks.
    auto zlib_compressor = std::make_unique<leveldb::ZlibCompressor>();
    options.compressors[1] = zlib_compressor.get();

    // create a reusable memory space for decompression so it allocates less
    leveldb::ReadOptions readOptions;
    auto decompress_allocator =
        std::make_unique<leveldb::DecompressAllocator>();
    readOptions.decompress_allocator = decompress_allocator.get();
    readOptions.verify_checksums = true;

    leveldb::Status status;

    std::string path = std::string(argv[1]) + "/db";

    leveldb::DB* pdb = nullptr;
    status = leveldb::DB::Open(options, path.c_str(), &pdb);
    auto db = std::unique_ptr<leveldb::DB>(pdb);

    if(!status.ok()) {
        fprintf(stderr, "ERROR: Opening '%s' failed.\n", path.c_str());
        return EXIT_FAILURE;
    }

    std::string key = decode_key(argv[2]);

#ifdef _WIN32
    fflush(stdin);
    setmode(fileno(stdin), O_BINARY);
#endif

    // slurp from cin stdin into value
    std::ostringstream slurp;
    slurp << std::cin.rdbuf();

    status = db->Put({}, key, slurp.str());

    if(!status.ok()) {
        fprintf(stderr, "ERROR: Reading key '%s' failed: %s\n", argv[2],
                status.ToString().c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
