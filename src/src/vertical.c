#include "convert.h"

void read_sent_vert(ifp)
/* read tags from file, reduce each word to lower case */
FILE	*ifp;
{
char	c, d, tempref[REFSIZE], temptag[SYNSIZE];
char	ref[REFSIZE], temporig[LGWORDSIZE], code[DECODESIZE];
char	split[5];
claws_line_tags_ent	*tags = NULL, *p = NULL;
int	i, x, found, cur;

    length = 0;
    cur = NIL;
    do
        {
        cur++;
        init_claws_line(ref, temporig, code, tags);
        infile_empty = read_claws_line2(ifp, ref, temporig, code, &tags, split);
        if (infile_empty)
            {
            /* rep_err("*** at EOF ***"); */
            cur--;
            break;
            }

        strcpy(buffer[cur].ref, ref);
        strcpy(buffer[cur].word, temporig);
        buffer[cur].wdln = strlen(buffer[cur].word);
        /* for CUP version we need all tags */
        if (cup_text)
            {
            /* for CUP put the enclitic marker just before the first tag */
            /* copy the first tag */
            sprintf(buffer[cur].syntag, "%s %s", split, tags->tag);
            if (tags->pct[0] != EOS)
                {
                strcat(buffer[cur].syntag, "/");
                strcat(buffer[cur].syntag, tags->pct);
                }
            /* copy all the tags into the output */
            p = tags->next;
            while (p)
                {
                strcat(buffer[cur].syntag, " ");
                strcat(buffer[cur].syntag, p->tag);
                if (p->pct[0] != EOS)
                    {
                    strcat(buffer[cur].syntag, "/");
                    strcat(buffer[cur].syntag, p->pct);
                    }
                p = p->next;
                }
            }
        else
            {
            /* copy the first tag */
            strcpy(buffer[cur].syntag, tags->tag);
            }

        /* show different CLAWS tags in parallel format */
        if (parallel && tags->next)
            {
            if (!strcmp(tags->next->tag, "|"))
                {
                if (tags->next->next)
                    {
                    char  temps[LGWORDSIZE];
                    sprintf(temps, "%s~%s", buffer[cur].syntag,
                        tags->next->next->tag);
                    strcpy(buffer[cur].syntag, temps);
                    }
                }
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

}	/* end of read_sent_vert */


void write_sent_vert(ofp, length, no_sos)
/* write current word, syntax tag and semantic tags to 'outfile' */
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
        if (is_supp_ref(buffer[cur].word) && vert_vert_supp)
            {
            get_string_from_supp(buffer[cur].word, tempsupp);
            /* if vertical to vertical conversion preserve ref nos */
            if (cup_text)
                fprintf(ofp, "%-25s %-8s", tempsupp, buffer[cur].syntag);
            else
                fprintf(ofp, "%-11s %-43s 00 %-8s", buffer[cur].ref,
                tempsupp, buffer[cur].syntag);
            }
        else
            {
            /* if vertical to vertical conversion preserve ref nos */
            if (vert_vert_supp)
                {
                if (cup_text)
                    fprintf(ofp, "%-25s %-8s", buffer[cur].word, buffer[cur].syntag);
                else
                    fprintf(ofp, "%-11s %-43s 00 %-8s", buffer[cur].ref,
                    buffer[cur].word, buffer[cur].syntag);
                }
            else
                {
                fprintf(ofp, "%-7s %3s %-43s 00 %-8s", temp1, temp2,
                    buffer[cur].word, buffer[cur].syntag);
                }
            }
        fprintf(ofp, "\n");
        }

}	/* end of write_sent_vert */


int is_supp_ref(s1)
/* check that string conforms to CLAWS supp file refs
eg **23;124;TOOLONG */ 
char	*s1;
{
int	i, x;

    /* string must be at length 6 */
    if (strlen(s1) < 6) return(FALSE);
    /* first 2 chars must be asterisk */
    if (!(s1[0] == '*' && s1[1] == '*')) return(FALSE);
    /* next a sequence of numbers before a semi colon */
    i = 2;
    while (isdigit(s1[i])) i++;
    if (i == 2 || s1[i] != ';') return(FALSE);
    /* same again */
    i++; x = i;
    while (isdigit(s1[i])) i++;
    if (i == x || s1[i] != ';') return(FALSE);
    /* could check remaining string here as well */

    return(TRUE);

}	/* end of is_supp_ref */

