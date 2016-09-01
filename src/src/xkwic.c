#include "convert.h"

void write_sent_xkwic(ofp, length, no_sos)
/* write current word, syntax tag 'outfile' */
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
        fprintf(ofp, "<s>\n");
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
        if (is_supp_ref(buffer[cur].word) && (vert_vert_supp || vert_xkwic_supp))
            {
            get_string_from_supp(buffer[cur].word, tempsupp);
            /* if vertical to vertical conversion preserve ref nos */
            fprintf(ofp, "%s\t%s", tempsupp, buffer[cur].syntag);
            }
        else
            {
            fprintf(ofp, "%s\t%s", buffer[cur].word, buffer[cur].syntag);
            }
        fprintf(ofp, "\n");
        }

}	/* end of write_sent_xkwic */


void write_semsent_xkwic(ofp, length, no_sos)
/* write current word, syntax tag and semantic tag to 'outfile' */
FILE	*ofp;
int	length;
int	no_sos;
{
int	cur;
char	tempsupp[10000];

    if (!no_sos)
        fprintf(ofp, "<s>\n");
    for (cur = 0; cur <= length; cur++)
        {
        if (strcmp(SBREAKTAG, buffer[cur].syntag) == 0)
            {
            continue;
            }
        /* write ref, syntag and original word */
        /* check for supp file references */
        if (is_supp_ref(buffer[cur].word) && vert_vert_supp)
            {
            get_string_from_supp(buffer[cur].word, tempsupp);
            /* if vertical to vertical conversion preserve ref nos */
            fprintf(ofp, "%s\t%s", tempsupp, buffer[cur].syntag);
            }
        else if (is_supp_ref(buffer[cur].word) && semantic_xkwic_supp)
            {
            get_string_from_supp(buffer[cur].word, tempsupp);
            fprintf(ofp, "%s\t%s\t%s", tempsupp, buffer[cur].syntag, buffer[cur].semtag);
            }
        else
            {
            fprintf(ofp, "%s\t%s\t%s", buffer[cur].word, buffer[cur].syntag,
		buffer[cur].semtag);
            }
        fprintf(ofp, "\n");
        }

}	/* end of write_semsent_xkwic */


