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

The C7 to C5 tagset mapping file (in the `resources` folder) can be used independently 
or in combination with the format change, for example, to map vertical C7 output to the C5 tagset, 
but retain the vertical format:

```
./convert -v2vmap taggedfile.c7 taggedfile.c5 mapC7toC5.txt
```

Or, to map vertical C7 CLAWS output to horizontal C5 format:

```
./convert -v2hsuppmap taggedfile.c7 taggedfile_horiz.c5 taggedfile.c7.supp mapC7toC5.txt
```
