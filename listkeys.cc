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

#include "db.hpp"
#include "mcbekey.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s <minecraft_world_dir> > list.tsv\n", argv[0]);
        return EXIT_FAILURE;
    }

    // construct path for Minecraft BE database
    std::string path = std::string(argv[1]) + "/db";

    // open the database
    mcberepair::DB db{path.c_str()};

    if(!db) {
        fprintf(stderr, "ERROR: Opening '%s' failed.\n", path.c_str());
        return EXIT_FAILURE;
    }

    // Print header
    printf("key\tbytes\tdimension\tx\tz\ttag\tsubtag\n");

    // create a reusable memory space for decompression so it allocates less
    leveldb::ReadOptions readOptions;
    leveldb::DecompressAllocator decompress_allocator;
    readOptions.decompress_allocator = &decompress_allocator;
    readOptions.verify_checksums = true;

    // create an iterator for the database
    auto it = std::unique_ptr<leveldb::Iterator>{db().NewIterator(readOptions)};

    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        auto key = it->key();
        // print an encoded key
        std::string enckey = mcberepair::encode_key({key.data(),key.size()});
        printf("%s", enckey.c_str());
        printf("\t%lu", it->value().size());

        // Identify keys that might represent chunks
        if(mcberepair::is_chunk_key({key.data(),key.size()})) {
            // read chunk key
            auto chunk = mcberepair::parse_chunk_key({key.data(),key.size()});
            // print chunk information
            printf("\t%d\t%d\t%d\t%d\t", chunk.dimension, chunk.x, chunk.z, chunk.tag);
            if(chunk.subtag != -1) {
                printf("%d", chunk.subtag);
            }
        } else {
            printf("\t\t\t\t\t");
        }

        printf("\n");
    }

    if(!it->status().ok()) {
        fprintf(stderr, "ERROR: Reading '%s' failed: %s\n", path.c_str(),
                it->status().ToString().c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
