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

#include "db.hpp"

#include "mcbekey.hpp"

int rmkeys_main(int argc, char *argv[]) {
    if(argc < 3 || strcmp("help", argv[1])==0) {
        printf("Usage: %s rmkeys <minecraft_world_dir> < keys.txt\n", argv[0]);
        printf("       %s rmkeys <minecraft_world_dir> <key> <key>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // construct path for Minecraft BE database
    std::string path = std::string(argv[2]) + "/db";

    // open the database
    mcberepair::DB db{path.c_str()};

    if(!db) {
        fprintf(stderr, "ERROR: Opening '%s' failed.\n", path.c_str());
        return EXIT_FAILURE;
    }

    leveldb::Status status;

    // Create a function that deletes the key.
    auto delete_key = [&](const std::string &line) -> bool {
        printf("Deleting key '%s'...\n", line.c_str());
        status = db().Delete({}, mcberepair::decode_key(line));
        if(!status.ok()) {
            fprintf(stderr, "ERROR: Writing '%s' failed: %s\n", path.c_str(),
                    status.ToString().c_str());
            return false;
        }
        return true;
    };

    // handle keys passed as arguments
    if(argc > 3) {
        for(int i=3;i<argc;++i) {
            if(!delete_key(argv[i])) {
                return EXIT_FAILURE;
            }
        }
        return EXIT_SUCCESS;
    }

    // Or handle keys passed on stdin.
    std::string line;
    while(std::getline(std::cin, line)) {
        if(!delete_key(line)) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
