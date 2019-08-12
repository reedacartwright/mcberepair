# mcberepair

## Backups

**Backup all minecraft worlds before using these tools.**
Editing your save games can be really dangerous and have unexpected results.

## listkeys

`listkeys` lists all the keys in a world's leveldb database. Output is a tab-separated file
with seven columns and a header.
Keys are [base64url encoded](https://en.wikipedia.org/wiki/Base64)
and placed in column 1. Keys that can be represented in plain text are prefixed with `plain:`
to distinguish them from the encoded keys. 
Column 2 holds the size of the data held by `key` in bytes.
If `key` looks like it represents a chunk, the chunk information will be parsed
and placed in columns 3--7.

### Example Output

```
key	bytes	x	z	dimension	tag	subchunk
cAEAACMAAAAvAA==	2547	368	35	0	47	0
cAEAACMAAAAvAQ==	4312	368	35	0	47	1
cAEAACMAAAAvAg==	4013	368	35	0	47	2
cAEAACMAAAAvAw==	589	368	35	0	47	3
cAEAACMAAAA2	4	368	35	0	54	
cAEAACMAAAB2	1	368	35	0	118	
plain:portals	2301					
cP___wH___8t	768	-144	-255	0	45	
cP___wH___8vAA==	4014	-144	-255	0	47	0
cP___wH___8vAQ==	2429	-144	-255	0	47	1
cP___wH___8vAg==	3129	-144	-255	0	47	2
cP___wH___8vAw==	2500	-144	-255	0	47	3
cP___wH___8y	1658	-144	-255	0	50	
cP___wH___82	4	-144	-255	0	54	
cP___wH___92	1	-144	-255	0	118	
```

## rmkeys

`rmkeys` deletes keys in a world's leveldb database.
Input is a list of keys, one per line.

### Example Input

```
plain:Nether
plain:portals
AAAAANj___8BAAAALQ==
AAAAANj___8BAAAALwA=
AAAAANj___8BAAAALwE=
AAAAANj___8BAAAALwI=
AAAAANj___8BAAAALwc=
AAAAANj___8BAAAAMg==
AAAAANj___8BAAAANg==
AAAAANj___8BAAAAdg==
AAAAANn___8BAAAALQ==
AAAAANn___8BAAAALwA=
```

## dumpkey

Dumps the binary contents of a value to stdout.

## writekey

Puts a value into the database. Reads binary data from stdin.

## Examples

This example runs in a Windows Command Prompt, and uses `gawk` to filter the output of listkeys
and generate the input to rmkeys.
`t5BPXQwUAQA=` is the path to the minecraft world folder that is being edited.

### Reset the Nether

```
listkeys "t5BPXQwUAQA=" > list.tsv
gawk "$5 == 1 {print $1}" list.tsv > netherkeys.txt

rmkeys "t5BPXQwUAQA=" < netherkeys.txt
echo portals|rmkeys "t5BPXQwUAQA="
echo Nether|rmkeys "t5BPXQwUAQA="
```

### Reset the End

```
listkeys "t5BPXQwUAQA=" > list.tsv
gawk "$5 == 2 {print $1}" list.tsv > endkeys.txt

rmkeys "t5BPXQwUAQA=" < endkeys.txt
echo TheEnd|rmkeys "t5BPXQwUAQA="
```

### Reset Overworld chunks that are greater than 100 chunks from 0,0

```
listkeys "t5BPXQwUAQA=" > list.tsv
gawk "sqrt($3^2+$4^2) > 100 && $5 == 0 {print $1}" list.tsv > farkeys.txt

rmkeys "t5BPXQwUAQA=" < farkeys.txt
```

### Copy the data from one key to another

```
dumpkey "t5BPXQwUAQA=" "AAAAANj___8BAAAALwA=" > subchunk.bin
writekey "t5BPXQwUAQA=" "AAAAANj___8BAAAALwE=" < subchunk.bin
```

### Print keys that don't belong to a chunk

```
listkeys "t5BPXQwUAQA=" > list.tsv
gawk "$3 == \"\"" list.tsv
```

## References

 - https://github.com/Mojang/leveldb-mcpe
 - https://minecraft.gamepedia.com/Bedrock_Edition_level_format
 - https://sourceforge.net/projects/ezwinports/files/gawk-5.0.1-w32-bin.zip/download
 - https://gist.github.com/Tomcc/a96af509e275b1af483b25c543cfbf37
 - https://minecraft.gamepedia.com/User:FVbico/Bedrock_NBT
 - https://github.com/midnightfreddie/nbt2json
 - https://github.com/midnightfreddie/McpeTool
