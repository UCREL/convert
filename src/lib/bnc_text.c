/* assume BNC text format */

/* something like:
<stext complete=Y org=SEQ   decls='CN002 QN002 SN000 TN006'>
<align>
<loc id=DCKLC001> <loc id=DCKLC002> <loc id=DCKLC003> <loc id=DCKLC004>
<loc id=DCKLC005>
<loc id=DCKLC006> <loc id=DCKLC007> <loc id=DCKLC008> <loc id=DCKLC009>
<loc id=DCKLC00A>
</align>
<u who=DCKPS000>
<s n=001>
 <w ITJ>Yeah <w UNC>erm <pause> <w AT0>the <w AJ0>other <w UNC>er <pause>
 <w NN1>aspect <w PRF>of <w DT0>any <w NN1>discussion <w PRF>of <w NP0>Vienna
<pause> <w VBZ>is <w AT0>the <w UNC>er<c PUN>, <w VBZ>is <w NN1>discussion
<w PRF>of
*/

/* in this format ditto tagged items appear as
 <w CJC>Plus <w PNP>it<w VBZ>'s <w AV0>a little bit
This unit is returned as a_little_bit
*/

#define	BNC_WORD_TAG		"<w"
#define	BNC_PUNC_TAG		"<c"

#include <stdio.h>
#include "util.h"

int get_bnc_next_word(fp, word, syn, ditto_as_single)
/* read one word at a time from a raw text file */
/* return TRUE at end of file or for a format error */
FILE	*fp;			/* file pointer to read from */
char	**word,			/* pointer to word!! */
	*syn;			/* CLAWS tag */
int	ditto_as_single;	/* flag to display ditto tagged items as single words */
{
char	c, d, temps[1000];
int	pos, templn;

    strcpy(syn, NULLTAG);
    pos = 0; templn = 0;

    c = getc(fp);
    /* get rid of any white space */
    while (isspace(c))
        c = getc(fp);

    while (!isspace(c) && c != EOF)
        {
        /* is it the start of an SGML tag */
        if (c == SGML_OPEN_DELIMITER)
            {
            temps[templn++] = c; c = getc(fp);
            /* read the SGML tag name */
            while (!isspace(c) && c != SGML_CLOSE_DELIMITER
            && c != EOF && templn < 999)
                {
                temps[templn++] = c; c = getc(fp);
                }
            temps[templn] = EOS;
            /* is it an indicator for the CLAWS tag? */
            if (!strcmp(temps, BNC_WORD_TAG) || !strcmp(temps, BNC_PUNC_TAG))
                {
                /* read the CLAWS tag */
                templn = 0; c = getc(fp);
                while (c != SGML_CLOSE_DELIMITER && c != EOF && templn < 999)
                    {
                    temps[templn++] = c; c = getc(fp);
                    }
                temps[templn] = EOS;
                strcpy(syn, temps);
/*puts(syn);*/
                /* now drop through to read the word */
                c = getc(fp);
                }
            else
                {
                /* must be a normal SGML tag so read the rest of it */
                while (c != SGML_CLOSE_DELIMITER && c != EOF && templn < 999)
                    {
                    temps[templn++] = c; c = getc(fp);
                    }
                temps[templn++] = SGML_CLOSE_DELIMITER; temps[templn] = EOS;
/*printf("%s %d\n", temps, templn);*/
                *word = STRING(temps);
                break;
                }
            }
        templn = 0;
        /* assume it is a normal word */
        while (!isspace(c) && c != SGML_OPEN_DELIMITER && c != EOF
        && templn < 999)
            {
            temps[templn++] = c; c = getc(fp);
            }
        temps[templn] = EOS;
        *word = STRING(temps);
/* printf("WORD %s\n", temps);*/
        if (c == SGML_OPEN_DELIMITER)
            /* no space between this and next SGML tag - probably punctuation */
            ungetc(c, fp);
        else if (ditto_as_single)
            {
            /* try getting the next char, if it is alphanumeric then this
            is probably a ditto tagged item */
            c = getc(fp);
            while (isalnum(c))
                {
                templn = 0;
                while (!isspace(c) && c != SGML_OPEN_DELIMITER && c != EOF
                && templn < 999)
                    {
                    temps[templn++] = c; c = getc(fp);
                    }
                temps[templn] = EOS;
/* printf("ADD %s %d\n", temps, templn);*/

                *word = STRAPP(*word, "_");
                *word = STRAPP(*word, temps);
                if (c == SGML_OPEN_DELIMITER || c == EOF)
                    break;
                else
                    c = getc(fp);
                }
            ungetc(c, fp);
            }
        break;
        }

    if (c == EOF)
        return(TRUE);
    else
        return(FALSE);

}	/* end of get_bnc_next_word */
