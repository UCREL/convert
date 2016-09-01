#include "convert.h"
#include <strings.h>

#define	UNDERSCORE 	'_'

void read_parse(ifp)
FILE	*ifp;
{
int	cur, finished = FALSE, parsing;
char	c, s1[100];

    cur = 0;
    length = 0;
    while ((fscanf(ifp, "%s", s1) != EOF) && (!finished))
        {
        if (member(UNDERSCORE, s1))
            {
            init_bufent(cur);
            separate_tag(s1, cur);
            cur++;
            }
        else if (member('[', s1) || member(']', s1))
                parsing = TRUE;
        else
            {
            c = getc(ifp);
            while ((c != EOLN) && (c != EOF))
                c = getc(ifp);
            finished = TRUE;
            break;
            }
        }
    length = cur - 1;
    if (length <= 0)
        read_parse(ifp);
    if (!finished)
        infile_empty = TRUE;

}	/* end of read_parse */
