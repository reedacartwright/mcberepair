# mcberepair

## Backups

**Backup all minecraft worlds before using these tools.**

## listkeys

`listkeys` lists all the keys in a world's leveldb database. Output is a tab-separated file
with seven columns and a header.
Keys are [percent encoded](https://en.wikipedia.org/wiki/Percent-encoding)
and placed in column 1.
Column 2 holds the size of the data held by `key` in bytes.
If `key` looks like it represents a chunk, the chunk information will be parsed
and placed in columns 3--7.

### Example Output

```
key	bytes	x	z	dimension	tag	subchunk
%00%00%00%00%00%00%00%00%02%00%00%00-	768	0	0	2	45	
%00%00%00%00%00%00%00%00%02%00%00%00/%00	593	0	0	2	47	0
%00%00%00%00%00%00%00%00%02%00%00%00/%01	559	0	0	2	47	1
%00%00%00%00%00%00%00%00%02%00%00%00/%02	559	0	0	2	47	2
%00%00%00%00%00%00%00%00%02%00%00%00/%03	1877	0	0	2	47	3
%00%00%00%00%00%00%00%00%02%00%00%00/%04	1139	0	0	2	47	4
%00%00%00%00%00%00%00%00%02%00%00%001	399	0	0	2	49	
%00%00%00%00%00%00%00%00%02%00%00%006	4	0	0	2	54	
%00%00%00%00%00%00%00%00%02%00%00%00v	1	0	0	2	118	
%00%00%00%00%01%00%00%00%02%00%00%00-	768	0	1	2	45	
%00%00%00%00%01%00%00%00%02%00%00%00/%00	593	0	1	2	47	0
%00%00%00%00%01%00%00%00%02%00%00%00/%01	559	0	1	2	47	1
%00%00%00%00%01%00%00%00%02%00%00%00/%02	559	0	1	2	47	2
%00%00%00%00%01%00%00%00%02%00%00%00/%03	593	0	1	2	47	3
%00%00%00%00%01%00%00%00%02%00%00%006	4	0	1	2	54	
%00%00%00%00%01%00%00%00%02%00%00%00v	1	0	1	2	118	
```

## rmkeys

`rmkeys` deletes keys in a world's leveldb database.
Input is a list of [percent encoded](https://en.wikipedia.org/wiki/Percent-encoding)
keys, one per line.

### Example Input

```
Nether
portals
%00%00%00%00%D8%FF%FF%FF%01%00%00%00-
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%00
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%01
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%02
%00%00%00%00%D8%FF%FF%FF%01%00%00%00/%07
```

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

### Print keys that don't belong to a chunk

```
listkeys "t5BPXQwUAQA=" > list.tsv
gawk "$3 == \"\"" list.tsv
```


## References

 - https://github.com/Mojang/leveldb-mcpe
 - https://minecraft.gamepedia.com/Bedrock_Edition_level_format
 - https://sourceforge.net/projects/ezwinports/files/gawk-5.0.1-w32-bin.zip/download
