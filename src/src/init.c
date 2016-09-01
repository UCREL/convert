#include "convert.h"

void init_bufent(buffervalue)
/* Initialise an entry in the buffer */
int	 buffervalue;
{
    buffer[buffervalue].syntag[0] = ENDOFSTR;
    buffer[buffervalue].word[0] = ENDOFSTR;
    strcpy(buffer[buffervalue].ref, "0000000");

}	/* end of init_bufent */


void initialise_punc()
/* Initialise the structure that holds the punctuation. */
{
    /* Set up array of punc items. */
    punc[0].item = STRING(".");
    punc[1].item = STRING(",");
    punc[2].item = STRING("?");
    punc[3].item = STRING("-");
    punc[4].item = STRING("...");
    punc[5].item = STRING("(");
    punc[6].item = STRING(")");
    punc[7].item = STRING("!");
    punc[8].item = STRING(":");
    punc[9].item = STRING(";");
    punc[10].item = STRING("\"");
    punc[11].item = STRING(SBREAKTAG);
    punc[12].item = STRING("&FO");

}	/* end of initialise_punc */
