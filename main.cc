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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "version.h"

int copyall_main(int argc, char *argv[]);
int dumpkey_main(int argc, char *argv[]);
int listkeys_main(int argc, char *argv[]);
int repair_main(int argc, char *argv[]);
int rmkeys_main(int argc, char *argv[]);
int writekey_main(int argc, char *argv[]);
int help_main(int argc, char *argv[]);

int version_main(int argc, char *argv[]) {
    printf("mcberepair v" MCBEREPAIR_VERSION "\n");
    return EXIT_SUCCESS;
}

using main_func_t = int (*)(int, char *argv[]);

struct command_t {
    const char *name;
    main_func_t main;
    const char *desc;
};

//clang-format off 
const command_t commands[] = {
    {"copyall",  copyall_main,  "Copy the entire contents from one world to an empty world."},
    {"dumpkey",  dumpkey_main,  "Dump the contents of a key to stdout."},
    {"listkeys", listkeys_main, "List the keys stored in the world."},
    {"repair",   repair_main,   "Run the database repair process on the world."},
    {"rmkeys",   rmkeys_main,   "Delete keys from the world."},
    {"writekey", writekey_main, "Set the contents of a key in the world."},
    {"help",     help_main,     "Print help information."},
    {"version",  version_main,  "Print version information."},
    {nullptr, nullptr, nullptr}};
//clang-format on 

int main(int argc, char *argv[]) {
    if(argc < 2) {
        help_main(argc, argv);
        return EXIT_FAILURE;
    }
    for(int i = 0; commands[i].name != nullptr; ++i) {
        if(strcmp(commands[i].name, argv[1]) == 0) {
            return commands[i].main(argc, argv);
        }
    }
    fprintf(stderr,
            "ERROR: Command '%s' not found. Try '%s help' for a list of "
            "commands.\n",
            argv[1], argv[0]);
    return EXIT_FAILURE;
}

int help_main(int argc, char *argv[]) {
    if(argc < 3 || strcmp("help", argv[2]) == 0) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        printf("\n");
        printf("Available Commands:\n");
        for(int i = 0; commands[i].name != nullptr; ++i) {
            printf("  %-10s %s\n", commands[i].name, commands[i].desc);
        }
        return EXIT_SUCCESS;
    }

    for(int i = 0; commands[i].name != nullptr; ++i) {
        if(strcmp(commands[i].name, argv[2]) == 0) {
            commands[i].main(argc, argv);
            return EXIT_SUCCESS;
        }
    }
    fprintf(stderr,
            "ERROR: Command '%s' not found. Try '%s help' for a list of "
            "commands.\n",
            argv[1], argv[0]);
    return EXIT_FAILURE;
}
