# mcberepair

mcberepair is a set of command line utilities for [Minecraft: Bedrock Edition](https://www.minecraft.net/en-us/about-minecraft) worlds.
This includes worlds generated on Windows 10, Android, XBox One, PS4, Switch, etc.
Data in a Minecraft: BE world is stored in a
[LevelDB](https://github.com/reedacartwright/leveldb-mcpe) database, and
mcberepair includes a set of utilities for manipulating these databases.

## Example Utilities

 - Listing all the keys in the db: `mcberepair listkeys`
 - Deleting a key in the db: `mcberepair rmkeys`
 - Dumping the contents of a key from the db: `mcberepair dumpkey`
 - Setting the contents of a key: `mcberepair writekey`
 - Repairing a db: `mcberepair repair`

## Backups

**Backup all minecraft worlds before using these tools.**
Editing your save games can be really dangerous and have unexpected results.

## Installation

Download the latest release from [mcberepair/releases](https://github.com/reedacartwright/mcberepair/releases). There are both binary releases for 64-bit Windows and source-code releases for other platforms.
Source-code releases also include a copy of [LevelDB](https://github.com/reedacartwright/leveldb-mcpe).

### Downloading via Git

If you clone the repo into a local copy, you should include submodules.

```sh
git clone --recurse-submodules https://github.com/reedacartwright/mcberepair.git
```

## Dependencies

 - leveldb-mcpe (included as a submodule)
 - zlib
 - [CMake](https://cmake.org/)

### Compiling mcberepair

If you have a recent version of CMake, you can compile it with the following commands.

```
cd path/to/mcberepair/source/code
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

After this runs successfully, you will have an `mcberepair` binary in your `./build` directory.
Run `./mcberepair help` from the build directory to see a list of available commands.

#### Compiling on Windows

Compiling mcberepair on Windows involves a few more steps than on Unix.

 1. Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)
 1. Install [Vcpkg](https://github.com/microsoft/vcpkg) and set up command line integration as described in the Vcpkg docs. Remember the location of Vcpkg's `CMAKE_TOOLCHAIN_FILE`
 1. Run `vcpkg install zlib:x64-windows`
 1. Open 'x64 Native Tools Command Propmpt for VS 2019' from your start menu and change your working directory to your the mcberepair source code directory.
 1. Run `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake`
 1. Run `cmake --build build --config Release --parallel`
 1. If this is successful, mcberepair can be found in `.\build\Release\mcberepair.exe`.


## Utility Documentation

mcberepair is a command line program and can be run from Powershell or a Windows Command Prompt; however, it will be more powerful if run inside a unix-like shell with unix tools. [BusyBox-win32](https://frippery.org/busybox/) is an easy way to get a suitable shell on Windows.

### listkeys

`mcberepair listkeys` lists all the keys in a world's leveldb database. Output is a tab-separated file
with seven columns and a header.
Plain text keys are [percent encoded](https://en.wikipedia.org/wiki/Percent-encoding) and placed in column 1.
Keys that represent chunk data being with `@` and are in the format
`@x:z:dimension:tag` or `@x:z:dimension:tag-subtag`
Column 2 holds the size of the data held by `key` in bytes.
If `key` looks like it represents a chunk, the chunk information will be parsed
and placed in columns 3--7.

#### Example Output

```
key	bytes	x	z	dimension	tag	subtag
@368:187:0:45	768	368	187	0	45	
@368:187:0:47-0	2586	368	187	0	47	0
@368:187:0:47-1	3094	368	187	0	47	1
@368:187:0:47-2	2468	368	187	0	47	2
@368:187:0:47-3	589	368	187	0	47	3
@368:187:0:54	4	368	187	0	54	
@368:187:0:118	1	368	187	0	118	
portals	10995					
@-144:0:2:45	768	-144	0	2	45	
@-144:0:2:47-0	2649	-144	0	2	47	0
@-144:0:2:47-1	2693	-144	0	2	47	1
@-144:0:2:47-2	2649	-144	0	2	47	2
@-144:0:2:47-3	3893	-144	0	2	47	3
@-144:0:2:47-4	3126	-144	0	2	47	4
@-144:0:2:51	132766	-144	0	2	51	
@-144:0:2:54	4	-144	0	2	54	
@-144:0:2:58	1510	-144	0	2	58	
@-144:0:2:118	1	-144	0	2	118	
```

### rmkeys

`mcberepair rmkeys` deletes keys in a world's leveldb database.
Input is a list of keys, one per line.

#### Example Input

```
Nether
portals
@0:0:2:45
@0:0:2:47-0
@0:0:2:47-1
@0:0:2:47-2
@0:0:2:47-3
@0:0:2:47-4
@0:0:2:49
@0:0:2:54
@0:0:2:118
```

### dumpkey

Dumps the binary contents of a value to stdout.

### writekey

Puts a value into the database. Reads binary data from stdin.

### repair

Attempts to fix a broken database and recover as much data as possible.

### copyall

Copies all data from one database to a fresh location.

## Examples

These examples run in a bash command prompt, and can be modified to run in a windows
command prompt.
Some use `awk` to filter the output of `mcberepair listkeys`.
Replace `t5BPXQwUAQA=` with a path to the minecraft world folder that is being edited.

### Reset the Nether and Portals

```
mcberepair listkeys t5BPXQwUAQA= > list.tsv
awk '$5 == 1 {print $1}' list.tsv > netherkeys.txt

mcberepair rmkeys t5BPXQwUAQA= < netherkeys.txt
mcberepair rmkeys t5BPXQwUAQA= portals Nether
```

### Reset the End

```
mcberepair listkeys t5BPXQwUAQA= > list.tsv
awk '$5 == 2 {print $1}' list.tsv > endkeys.txt

mcberepair rmkeys t5BPXQwUAQA= < endkeys.txt
mcberepair rmkeys t5BPXQwUAQA= TheEnd
```

### Reset Overworld chunks that are greater than 100 chunks from 0,0

```
mcberepair listkeys t5BPXQwUAQA= > list.tsv
awk '$5 == 0 && sqrt($3^2+$4^2) > 100 {print $1}' list.tsv > farkeys.txt

mcberepair rmkeys t5BPXQwUAQA= < farkeys.txt
```

### Copy the data from one key to another

```
mcberepair dumpkey t5BPXQwUAQA= '@0:0:2:47-0' > subchunk.bin
mcberepair writekey t5BPXQwUAQA= '@0:1:2:47-0' < subchunk.bin
```

### Print keys that don't belong to any chunk

```
mcberepair listkeys t5BPXQwUAQA= > list.tsv
awk '$3 == ""' list.tsv
```

### Delete all pending ticks records above 4096 bytes in size.

```
mcberepair listkeys t5BPXQwUAQA= > list.tsv
awk '$6 == 51 && $2 > 4096 {print $1}' list.tsv > pending_ticks.tsv
mcberepair rmkeys t5BPXQwUAQA= < pending_ticks.tsv
```

## References

 - https://github.com/Mojang/leveldb-mcpe
 - https://minecraft.gamepedia.com/Bedrock_Edition_level_format
 - https://sourceforge.net/projects/ezwinports/files/gawk-5.0.1-w32-bin.zip/download
 - https://gist.github.com/Tomcc/a96af509e275b1af483b25c543cfbf37
 - https://minecraft.gamepedia.com/User:FVbico/Bedrock_NBT
 - https://github.com/midnightfreddie/nbt2json
 - https://github.com/midnightfreddie/McpeTool
