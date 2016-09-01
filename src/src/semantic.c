#include "convert.h"

extern
linkent *semtag_strip_marker();

void read_semantic(ifp, strip_flag, pos_strip_flag)
/* read tags from file, reduce each word to lower case */
FILE	*ifp;
int	strip_flag,	/* flag to tell us whether to strip semantic tags
			and idioms for COOCUR program */
	pos_strip_flag;	/* flag to tell us whether to output only reduced set
			of POS (N/V/R/J) */
{
char	c, d, tempref[REFSIZE],
	tempsyn[SYNSIZE],
	orig[WORDSIZE],
	semtag[SEMSIZE],
	parse[LGWORDSIZE];
int	i, found;
int     cur, smno;
linkent	*link;
int	idcn, idno, idpo;

    length = 0;
    cur = NIL;
    do
        {
        cur++;
        smno = 0; link = NULL;
        init_bufent(cur);

        infile_empty = get_semtag_next_line(ifp, tempref, orig, tempsyn,
            semtag, parse, EXTRACT_FIRST_TAG);
        if (infile_empty)
            {
            cur--;
            break;
            }

        if (strip_flag)
            {
            /* work out whether we've got an idiom */
            link = semtag_strip_marker(semtag, &idcn, &idno, &idpo, TRUE, TRUE);
            /* TODO: free link */
            /* only record the first part of semantic idioms */
            if (idcn != 0)
                {
                if (idpo != 1)
                    {
                    /* reduce current position by one and go to the next word */
                    cur--;
                    continue;
                    }
                }
            if (pos_strip_flag)
                {
                /* only allow nouns, verbs, adjectives and adverbs */
                /* NULL is a special case */
                if (!strcmp(tempsyn, NULLTAG) || !strchr("NVJR", tempsyn[0])
                || !strcmp(semtag, "PUNC"))
                    {
                    /* reduce current position by one and go to the next word */
                    cur--;
                    continue;
                    }
                }
/*            chop_semtag_level_two(semtag);*/
            /* chop at level one */
            semtag[1] = EOS;
            }

        strcpy(buffer[cur].ref, tempref);
        /* pass semantic and POS tags */
        strcpy(buffer[cur].syntag, tempsyn);
        strcpy(buffer[cur].semtag, semtag);

        /* set counters */
        buffer[cur].sxln = strlen(buffer[cur].syntag);
        buffer[cur].wdln = strlen(orig);

        strcpy(buffer[cur].word, orig);
        }
    while ((!infile_empty) && (cur < BUFFERSIZE - 1)
    && (strcmp(buffer[cur].word, SBREAKTAG) != 0));
    /* end of sentence indicated by EOF, sentence too long , full-stop etc */
    length = cur;
/*    no_sentences++;*/

}	/* end of read_semantic */


void write_sentence(ofp)
/* write current word, syntax tag and semantic tags to 'outfile' */
FILE	*ofp;
{
int	i;
int     cur;

    for (cur=0; cur <= length; cur++)
        {
        /* write ref, syntag and original word */
        fprintf(ofp,"%-13s%-8s%-25.25s", buffer[cur].ref,
            buffer[cur].syntag, buffer[cur].word);

        /* output all semantic tags. */
/*        for (i=0 ; i < MAX_TAGSEM ; i++)
            {
            if (buffer[cur].semtag[i].data[0] != ENDOFSTR)
                fprintf(ofp,"%s ",buffer[cur].semtag[i].data);
            }
        fprintf(ofp,"\n");
*/
        }

}	/* end of write_sentence */

