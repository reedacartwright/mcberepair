#include <cassert>
#include <cstdio>
#include <memory>

#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "leveldb/decompress_allocator.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/zlib_compressor.h"

#include "mcbekey.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s <minecraft_world_dir> > list.tsv\n", argv[0]);
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

    // Print header
    printf("key\tbytes\tx\tz\tdimension\ttag\tsubchunk\n");

    auto it = std::unique_ptr<leveldb::Iterator>{db->NewIterator(readOptions)};

    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        auto k = it->key();
        // print an encoded key
        std::string key = encode_key(k.data(), k.size());
        printf("%s", key.c_str());
        printf("\t%lu", it->value().size());

        // Identify keys that might represent chunks
        // See https://minecraft.gamepedia.com/Bedrock_Edition_level_format
        if(k.size() == 9 || k.size() == 10 || k.size() == 13 ||
           k.size() == 14) {
            // read x and z coordinates
            int x; //= //k[0] | (k[1] << 8) | (k[2] << 16) | (k[3] << 24);
            int z; //= k[4] | (k[5] << 8) | (k[6] << 16) | (k[7] << 24);
            memcpy(&x, k.data()+0, 4);
            memcpy(&z, k.data()+4, 4);
            // read the dimension coordinate
            int d = 0;
            if(k.size() >= 13) {
                memcpy(&d, k.data()+8, 4);
            }
            // read the tag
            int tag = (k.size() >= 13) ? k[12] : k[8];
            // read the subchunk
            int subchunk = -1;
            if(k.size() == 10) {
                subchunk = k[9];
            } else if(k.size() == 14) {
                subchunk = k[13];
            }
            // Print information if tag is valid
            if((45 <= tag && tag <= 58) || tag == 118) {
                printf("\t%d\t%d\t%d\t%d\t", x, z, d, tag);
                if(subchunk != -1) {
                    printf("%d", subchunk);
                }
            } else {
                printf("\t\t\t\t\t");
            }
        } else {
            printf("\t\t\t\t\t");
        }

        printf("\n");
    }

    if(!status.ok()) {
        fprintf(stderr, "ERROR: Reading '%s' failed: %s\n", path.c_str(),
                status.ToString().c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
