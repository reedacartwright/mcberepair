# mcberepair

## Backups

**Backup all minecraft worlds before using these tools.**

## listkeys

`listkeys` lists all the keys in a world's leveldb database. Output is a tab-separated file with six columns and a header.
Keys are [percent encoded](https://en.wikipedia.org/wiki/Percent-encoding).
If the key looks like it represents a chunk, the chunk information will be parsed and placed in columns 2-6.
Example output:

```
key	x	z	dimension	tag	subchunk
%00%00%00%00%00%00%00%00%02%00%00%00-	0	0	2	45	
%00%00%00%00%00%00%00%00%02%00%00%00/%00	0	0	2	47	0
%00%00%00%00%00%00%00%00%02%00%00%00/%01	0	0	2	47	1
%00%00%00%00%00%00%00%00%02%00%00%00/%02	0	0	2	47	2
%00%00%00%00%00%00%00%00%02%00%00%00/%03	0	0	2	47	3
%00%00%00%00%00%00%00%00%02%00%00%00/%04	0	0	2	47	4
%00%00%00%00%00%00%00%00%02%00%00%001	0	0	2	49	
%00%00%00%00%00%00%00%00%02%00%00%006	0	0	2	54	
%00%00%00%00%00%00%00%00%02%00%00%00v	0	0	2	118	
```

## rmkeys

`rmkeys` deletes keys in a world's leveldb database. Input is a list of [percent encoded](https://en.wikipedia.org/wiki/Percent-encoding)
keys, one per line. Example input:

```
Nether
portals
%00%00%00%00%D8%FF%FF%FF%01%00%00%00-
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%00
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%01
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%02
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%07
```

## Example: Reset the nether

This example runs in a Windows Command Prompt, and uses `gawk` to create a list of keys from the nether dimension.

```
listkeys "t5BPXQwUAQA=" > list.tsv
gawk "$4 == 1 {print $1}" list.tsv > netherkeys.txt

rmkeys "t5BPXQwUAQA=" < netherkeys.txt
echo portals|rmkeys "t5BPXQwUAQA="
echo Nether|rmkeys "t5BPXQwUAQA="
```

## References

 - https://github.com/Mojang/leveldb-mcpe
 - https://minecraft.gamepedia.com/Bedrock_Edition_level_format
 - https://sourceforge.net/projects/ezwinports/files/gawk-5.0.1-w32-bin.zip/download
