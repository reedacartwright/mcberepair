#include <cassert>
#include <iostream>
#include <iomanip>

#include "leveldb/db.h"
#include "leveldb/zlib_compressor.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/cache.h"
#include "leveldb/decompress_allocator.h"


using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Usage: delnether <leveldb_dir>\n";
        return 1;
    }

    class NullLogger : public leveldb::Logger {
    public:
        void Logv(const char*, va_list) override {
        }
    };

    leveldb::Options options;
    
    //create a bloom filter to quickly tell if a key is in the database or not
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);

    //create a 40 mb cache (we use this on ~1gb devices)
    options.block_cache = leveldb::NewLRUCache(40 * 1024 * 1024);

    //create a 4mb write buffer, to improve compression and touch the disk less
    options.write_buffer_size = 4 * 1024 * 1024;

    //disable internal logging. The default logger will still print out things to a file
    //options.info_log = new NullLogger();

    //use the new raw-zip compressor to write (and read)
    options.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
    
    //also setup the old, slower compressor for backwards compatibility. This will only be used to read old compressed blocks.
    options.compressors[1] = new leveldb::ZlibCompressor();

    
    //create a reusable memory space for decompression so it allocates less
    leveldb::ReadOptions readOptions;
    readOptions.decompress_allocator = new leveldb::DecompressAllocator();

    leveldb::Status status;
    leveldb::DB* db;

    status = leveldb::DB::Open(options, argv[1], &db);
    if(!status.ok()) {
        cerr << "Opening " << argv[1] << " failed." << endl;
        return 1;
    }

    readOptions.verify_checksums = true;
    leveldb::Iterator* it = db->NewIterator(readOptions);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        auto k = it->key();
    
        if(k.size() == 13 || k.size() == 14) {
            int x = k[0] | (k[1] << 8) | (k[2] << 16) | (k[3] << 24);
            int z = k[4] | (k[5] << 8) | (k[6] << 16) | (k[7] << 24);
            int d = k[8] | (k[9] << 8) | (k[10] << 16) | (k[11] << 24);

            if(d != 1) {
                continue;
            }

            cout << "Deleting key ";
            for(int i=0;i<k.size();++i) {
                cout << hex << setfill('0') << setw(2)
                     << static_cast<unsigned int>(k[i] & 0xFF);
            }
            cout << "\n";
            status = db->Delete(leveldb::WriteOptions(), k);
            if(!status.ok()) {
                cerr << status.ToString() << endl;
                return 1;
            }
        } 
    }

    status = db->Delete(leveldb::WriteOptions(), "portals");
    status = db->Delete(leveldb::WriteOptions(), "Nether");
    
    if(!status.ok()) {
        cerr << status.ToString() << endl;
        return 1;
    }


    if(!status.ok()) {
        cerr << "Reading " << argv[1] << " failed." << endl;
        return 1;
    }
    delete it;
    delete db;
    return 0;
}
