#include "convert.h"

void read_bnc_text(ifp)
/* read sentence of text from file */
FILE	*ifp;
{
char	*temporig, syn[LGWORDSIZE], temp[1000];
int	cur, wdln, i, j, parts, thispart;

    length = 0;
    cur = NIL;
    do
        {
        cur++;
        infile_empty = get_bnc_next_word(ifp, &temporig, syn, FALSE);
        if (infile_empty)
            {
            cur--;
            break;
            }

        /* check for ditto tagged items */
        /* these are passed back from get_bnc_next_word
        as ever_so and at_all and need to split */
        if (strchr(temporig, '_'))
            {
            parts = 1;
            wdln = strlen(temporig);
            /* find out how many parts */
            for (i = 0; i < wdln; i++)
                {
                if (temporig[i] == '_') parts++;
                }
            /* copy out and split */
            thispart = 1; j = 0;
            for (i = 0; i < wdln; i++)
                {
                if (temporig[i] == '_')
                    {
                    buffer[cur].word[j] = EOS;
                    if (!strcmp(syn, "NULL"))
                        strcpy(buffer[cur].syntag, syn);
                    else
                        sprintf(buffer[cur].syntag, "%s%d%d", syn, parts, thispart);
                    buffer[cur].wdln = strlen(buffer[cur].word);
                    buffer[cur].sxln = strlen(buffer[cur].syntag);
                    strcpy(buffer[cur].ref, "");
                    cur++; j = 0; thispart++;
                    }
                else
                    {
                    buffer[cur].word[j] = temporig[i];
                    j++;
                    }
                }
            buffer[cur].word[j] = EOS;
            if (!strcmp(syn, "NULL"))
                strcpy(buffer[cur].syntag, syn);
            else
                sprintf(buffer[cur].syntag, "%s%d%d", syn, parts, thispart);
            buffer[cur].wdln = strlen(buffer[cur].word);
            buffer[cur].sxln = strlen(buffer[cur].syntag);
            strcpy(buffer[cur].ref, "");
            }
        else
            {
            strcpy(buffer[cur].ref, "");
            buffer[cur].wdln = strlen(temporig);
            strcpy(buffer[cur].syntag, syn);
            buffer[cur].sxln = strlen(buffer[cur].syntag);
            strcpy(buffer[cur].word, temporig);
            }

        myfree(temporig);
        }
    while ((!infile_empty) && (cur < BUFFERSIZE-1)
    && (strcmp(buffer[cur].syntag, SBREAKTAG))
    && (!strchr(".?!", buffer[cur].word[0])));
    /* end of sentence indicated by EOF, sentence too long , full-stop etc */
    length = cur;

}	/* end of read_bnc_text */
