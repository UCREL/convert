/* assume raw text format - ie no SGML tags */

#include <stdio.h>
#include "util.h"

int get_sgmlraw_text_word(fp, word)
/* read one word at a time from a raw text file which includes SGML tags*/
/* return TRUE at end of file or for a format error */
FILE	*fp;			/* file pointer to read from */
char	*word;			/* word!! */
{
char	c, d;
int	pos;

    pos = 0;
    c = getc(fp);
    /* get rid of any white space */
    while (isspace(c))
        c = getc(fp);

    while (!isspace(c) && c != EOF)
        {
        /* add to string if it is not punctuation */
        if (!strchr(".,?-()!;:[]<>^", c))
            word[pos++] = c;
        else if (c == '<')
            {
            /* this may be the start of an SGML tag */
            /* in any case we should break the current word at this point */
            /* if the next character is not alphanumeric or ?!/ we assume it is not */
            if (pos == 0)
                {
                /* add to string if at start of word */
                word[pos++] = c;
                /* check next char */
                d = getc(fp);
                if (!isalnum(d) && !strchr("?!/" ,d))
                    {
                    ungetc(d, fp);
/*                    word[pos] = EOS;
                    printf ("'%s'\n", word);
                    getc(stdin);
*/
                    break;
                    }
                else
                    {
                    /* add it to string and then continue adding until
                    end of SGML tag */
                    word[pos++] = d;
                    d = getc(fp);
                    while (d != '>' && pos < (LGWORDSIZE - 5))
                        {
                        word[pos++] = d;
                        d = getc(fp);
                        }
                    /* add final bracket and then break word */
                    word[pos++] = d;
/*                    word[pos] = EOS;
                    printf ("'%s'\n", word);
                    getc(stdin);
*/
                    break;
                    }
                }
            else
                {
                /* otherwise split off */
                ungetc(c, fp);
                break;
                }
            }
        /* the character is a punctuation symbol */
        else
            {
            d = getc(fp);
            if (isalnum(d) && pos != 0)
                {
                /* add to string if word internal weirdo */
                word[pos++] = c;
                word[pos++] = d;
                }
            else if (pos == 0)
                {
                /* add to string if at start of word
                (to stop never ending loop) */
                /* but then break word */
                word[pos++] = c;
                ungetc(d, fp);
                break;
                }
            else
                {
                /* otherwise split off */
                ungetc(d, fp); ungetc(c, fp);
                break;
                }
            }
        c = getc(fp);
        }

    word[pos] = EOS;

    if (c == EOF)
        return(TRUE);
    else
        return(FALSE);

}	/* end of get_sgmlraw_text_word */
