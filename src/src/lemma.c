#include "convert.h"

void read_sent_lemma(ifp)
/* read lemmatised text of the format:
0000192 050  NP1              Horatio                  horatio
0000192 051  NULL             </rs>                    PUNC
*/
FILE	*ifp;
{
char	c, d, tempref[REFSIZE], temptag[SYNSIZE], templem[LGWORDSIZE];
char	ref[REFSIZE], temporig[LGWORDSIZE], code[DECODESIZE];
claws_line_tags_ent	*tags = NULL;
int	i, x, found, cur;

    length = 0;
    cur = NIL;
    do
        {
        cur++;
        strcpy(ref, "");
        strcpy(temporig, "");
        strcpy(code, "");
        if (fscanf(ifp, "%s %s %s %s %s", ref, tempref, temptag, temporig, templem) == EOF)
            infile_empty = TRUE;
        if (infile_empty)
            {
            /* rep_err("*** at EOF ***"); */
            cur--;
            break;
            }

        /* check for words split by CLAWS */
        /* can't reform something at start of sentence */
        if (tempref[2] != '0' && strcmp(templem, "PUNC") && (cur > 0))
            {
            if (cur > 0) cur--;
            /* leave ref number as for previous word */
            /* concatenate next part of word onto previous */
            strcat(buffer[cur].word, temporig);
            buffer[cur].wdln = strlen(buffer[cur].word);
            /* concatenate tag and lemma with hyphen */
            strcat(buffer[cur].syntag, "-");
            strcat(buffer[cur].syntag, temptag);
            strcat(buffer[cur].lemma, "-");
            strcat(buffer[cur].lemma, templem);
            }
        else
            {
            /* reformat 3 digit group */
            char temp5[LGWORDSIZE];
            sprintf(temp5, "0%-2.2s", tempref);
            sprintf(buffer[cur].ref, "%s %s", ref, temp5);
            /* replace ref nos that don't increment counter */
            if (tempref[2] != '0')
                sprintf(buffer[cur].ref, "%s NULL", ref);
            strcpy(buffer[cur].word, temporig);
            buffer[cur].wdln = strlen(buffer[cur].word);
            strcpy(buffer[cur].syntag, temptag);
            strcpy(buffer[cur].lemma, templem);
            }
            
        /* set counters */
        buffer[cur].sxln = strlen(buffer[cur].syntag);

        /* delete @ and % markers on tags */
        if (remove_rarity
        && strchr("@%", buffer[cur].syntag[buffer[cur].sxln-1]))
            {
            buffer[cur].syntag[buffer[cur].sxln-1] = EOS;
            buffer[cur].sxln--;
            }

        }
    while ((!infile_empty) && (cur < BUFFERSIZE-1)
/*    && (!member(buffer[cur].syntag[0],".!?"))*/
    && (strcmp(buffer[cur].word, SBREAKTAG)));
    /* end of sentence indicated by EOF, sentence too long , full-stop etc */
    length = cur;

}	/* end of read_sent_lemma */


void write_sent_lemma(ofp, length, no_sos)
/* write current word, syntax tag and lemmas to 'outfile' */
FILE	*ofp;
int	length;
int	no_sos;
{
int	cur;
char	temp1[8], temp2[5];
char	tempsupp[10000];

    sentctr += 1;
    itemctr = 1;
    temp1[0] = ENDOFSTR;
    temp2[0] = ENDOFSTR;
    itostr(sentctr, temp1, 7);
    if (!no_sos)
        fprintf(ofp, "%-7s 001 %s\n", temp1, sentbreak);
    for (cur = 0; cur <= length; cur++)
        {
        /* write ref, syntag and original word */
        temp2[0] = ENDOFSTR;
        if (is_punc(buffer[cur].syntag))
            itemctr += 1;
        else
            {
            itemctr += 10;
            itemctr = itemctr / 10;
            itemctr = itemctr * 10;
            }
        if (itemctr >= 1000)
            itemctr = 0;
        itostr(itemctr, temp2, 3);

        /* check for supp file references */
        if (is_supp_ref(buffer[cur].word))
            {
            int i;
            get_string_from_supp(buffer[cur].word, tempsupp);
            /* replace eolns in supp string with spaces */
            for (i = 0; i < strlen(tempsupp); i++)
                {
                if (tempsupp[i] == '\n') tempsupp[i] = ' ';
                }
            /* if vertical to vertical conversion preserve ref nos */
            fprintf(ofp, "%-13s%-17s%-24s %-10s ", buffer[cur].ref,
                buffer[cur].syntag, tempsupp, buffer[cur].lemma);
            }
        else
            {
/*                fprintf(ofp, "%-7s %3s %-43s 00 %-8s", temp1, temp2,
                    buffer[cur].word, buffer[cur].syntag);
*/
            fprintf(ofp, "%-13s%-17s%-24s %-10s ", buffer[cur].ref,
                buffer[cur].syntag, buffer[cur].word, buffer[cur].lemma);
            }
        fprintf(ofp, "\n");
        }

}	/* end of write_sent_lemma */
