cl /std:c++14 /EHsc /O2 /I../leveldb-mcpe/include /D LEVELDB_PLATFORM_WINDOWS /D "DLLX=_declspec(dllimport)" listkeys.cc ../leveldb-mcpe/x64/Release/leveldb.lib
cl /std:c++14 /EHsc /O2 /I../leveldb-mcpe/include /D LEVELDB_PLATFORM_WINDOWS /D "DLLX=_declspec(dllimport)" rmkeys.cc ../leveldb-mcpe/x64/Release/leveldb.lib