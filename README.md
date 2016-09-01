# Convert

A C program written by Paul Rayson to convert between many of the 
different file formats in the UCREL NLP pipeline.

## Licence

This software is licenced under GPL version 3. It is available publicly for academic 
use but not commercially.

## Compilation

`cd` into the `src` folder and run `make`

## Running instructions

Run the software without any command line flags to see the different options:

```
./convert
```

For example, to convert from CLAWS vertical output format to word_TAG horizontal format:

```
./convert -v2hsupp taggedfile.c7 taggedfile_horiz.c7 taggedfile.c7.supp
```

where `taggedfile.c7` is the vertical output format from CLAWS 
(and input to `convert`),
`taggedfile_horiz.c7` is the horizontal word_TAG format 
(output from `convert`),
and `taggedfile.c7.supp` is the supplementary output file from CLAWS.
