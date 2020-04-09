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
#include <fcntl.h>
#include <io.h>
#endif

#include "db.hpp"
#include "mcbekey.hpp"
#include "slurp.hpp"

int writekey_main(int argc, char* argv[]) {
    if(argc < 4 || strcmp("help", argv[1]) == 0) {
        printf("Usage: %s writekey <minecraft_world_dir> <key> < input.bin\n",
               argv[0]);
        return EXIT_FAILURE;
    }

#ifdef _WIN32
    fflush(stdin);
    _setmode(_fileno(stdin), O_BINARY);
#endif

    // slurp from stdin into value before we open db
    auto value = mcberepair::slurp_string(std::cin);

    // construct path for Minecraft BE database
    std::string path = std::string(argv[2]) + "/db";

    // open the database
    mcberepair::DB db{path.c_str()};

    if(!db) {
        fprintf(stderr, "ERROR: Opening '%s' failed.\n", path.c_str());
        return EXIT_FAILURE;
    }

    std::string key = mcberepair::decode_key(argv[3]);

    leveldb::Status status = db().Put({}, key, value);

    if(!status.ok()) {
        fprintf(stderr, "ERROR: Reading key '%s' failed: %s\n", argv[3],
                status.ToString().c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
