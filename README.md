# mcberepair

## Backups

**Backup all minecraft worlds before using these tools.**
Editing your save games can be really dangerous and have unexpected results.

## listkeys

`listkeys` lists all the keys in a world's leveldb database. Output is a tab-separated file
with seven columns and a header.
Plain text keys are [percent encoded](https://en.wikipedia.org/wiki/Percent-encoding) and placed in column 1.
Keys that represent chunk data being with `@` and are in the format
`@dimension:x:z:tag` or `@dimension:x:z:tag-subtag`
Column 2 holds the size of the data held by `key` in bytes.
If `key` looks like it represents a chunk, the chunk information will be parsed
and placed in columns 3--7.

### Example Output

```
key	bytes	dimension x	z	tag	subchunk
@0:368:35:45    768 0   368 35  45  
@0:368:35:47-0  2547    0   368 35  47  0
@0:368:35:47-1  4312    0   368 35  47  1
@0:368:35:47-2  4013    0   368 35  47  2
@0:368:35:47-3  589 0   368 35  47  3
@0:368:35:54    4   0   368 35  54  
@0:368:35:118   1   0   368 35  118 
portals 2301                    
@0:-144:-255:45 768 0   -144    -255    45  
@0:-144:-255:47-0   4014    0   -144    -255    47  0
@0:-144:-255:47-1   2429    0   -144    -255    47  1
@0:-144:-255:47-2   3129    0   -144    -255    47  2
@0:-144:-255:47-3   2500    0   -144    -255    47  3
@0:-144:-255:50 1658    0   -144    -255    50  
@0:-144:-255:54 4   0   -144    -255    54  
@0:-144:-255:118    1   0   -144    -255    118 
```

## rmkeys

`rmkeys` deletes keys in a world's leveldb database.
Input is a list of keys, one per line.

### Example Input

```
Nether
portals
@2:0:0:45
@2:0:0:47-0
@2:0:0:47-1
@2:0:0:47-2
@2:0:0:47-3
@2:0:0:47-4
@2:0:0:49
@2:0:0:54
@2:0:0:118
```

## dumpkey

Dumps the binary contents of a value to stdout.

## writekey

Puts a value into the database. Reads binary data from stdin.

## Examples

These examples run in a bash command prompt, and can be modified to run in a windows
command prompt.
Some use `awk` to filter the output of `listkeys`.
Replace `t5BPXQwUAQA=` with a path to the minecraft world folder that is being edited.

### Reset the Nether and Portals

```
listkeys t5BPXQwUAQA= > list.tsv
awk '$3 == 1 {print $1}' list.tsv > netherkeys.txt

rmkeys t5BPXQwUAQA= < netherkeys.txt
rmkeys t5BPXQwUAQA= portals Nether
```

### Reset the End

```
listkeys t5BPXQwUAQA= > list.tsv
awk '$3 == 2 {print $1}' list.tsv > endkeys.txt

rmkeys t5BPXQwUAQA= < endkeys.txt
rmkeys t5BPXQwUAQA= TheEnd
```

### Reset Overworld chunks that are greater than 100 chunks from 0,0

```
listkeys t5BPXQwUAQA= > list.tsv
awk '$3 == 0 && sqrt($4^2+$5^2) > 100 {print $1}' list.tsv > farkeys.txt

rmkeys t5BPXQwUAQA= < farkeys.txt
```

### Copy the data from one key to another

```
dumpkey t5BPXQwUAQA= '@2:0:0:47-0' > subchunk.bin
writekey t5BPXQwUAQA= '@2:0:1:47-0' < subchunk.bin
```

### Print keys that don't belong to any chunk

```
listkeys t5BPXQwUAQA= > list.tsv
awk '$3 == ""' list.tsv
```

### Delete all pending ticks records above 4096 bytes in size.

```
listkeys t5BPXQwUAQA= > list.tsv
awk '$6 == 51 && $2 > 4096 {print $1}' list.tsv > pending_ticks.tsv
rmkeys t5BPXQwUAQA= < pending_ticks.tsv
```

## References

 - https://github.com/Mojang/leveldb-mcpe
 - https://minecraft.gamepedia.com/Bedrock_Edition_level_format
 - https://sourceforge.net/projects/ezwinports/files/gawk-5.0.1-w32-bin.zip/download
 - https://gist.github.com/Tomcc/a96af509e275b1af483b25c543cfbf37
 - https://minecraft.gamepedia.com/User:FVbico/Bedrock_NBT
 - https://github.com/midnightfreddie/nbt2json
 - https://github.com/midnightfreddie/McpeTool
