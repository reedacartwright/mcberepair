
# Location of the leveldb-mcpe directories 
# See https://github.com/Mojang/leveldb-mcpe
LEVELDB_ROOT?=leveldb-mcpe
LEVELDB_INCLUDE=-I$(LEVELDB_ROOT)/include
LEVELDB_LIB=$(LEVELDB_ROOT)/out-static/libleveldb.a \
            $(LEVELDB_ROOT)/out-static/libmemenv.a \
            -lz
LEVELDB_FLAGS=-fno-builtin-memcmp -pthread \
	-DOS_LINUX -DLEVELDB_PLATFORM_POSIX \
	-DLEVELDB_ATOMIC_PRESENT -DDLLX=

default: all

all: delnether

.PHONY: default all

delnether: delnether.cc
	$(CXX) -std=c++14 -o $@ $< $(LEVELDB_FLAGS) $(LEVELDB_INCLUDE) $(LEVELDB_LIB)
