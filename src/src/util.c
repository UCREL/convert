#include "convert.h"

void help()
/* print help message and stop */
{
    fputs("CONVERT:", stdout);
    fputs(UCREL_COPYRIGHT, stdout);
    fputs("VERSION ", stdout);
    puts(VERSION);
    printf("Usage:   convert -convert_type [-nosos|-sentnum|-rare|-fletter|-noeolnsupp] infile outfile [suppfile] [mappingfile]\n");
    printf("Options: -h2v convert from horizontal to vertical format\n");
    printf("         -h2k convert from horizontal to xkwic format\n");
    printf("         -v2h convert from vertical to horizontal format\n");
    printf("     -v2hsupp convert from vertical to horizontal format with supp file\n");
    printf("  -v2hsuppmap convert from vertical to horizontal format with supp file and POS tag mapping\n");
    printf("         -v2k convert from vertical to IMS-CWB (xkwic) format\n");
    printf("     -v2ksupp convert from vertical to IMS-CWB (xkwic) format with supp file\n");
    printf("     -v2vsupp convert from vertical to vertical format with supp file\n");
    printf("      -v2vmap convert from vertical to vertical format with POS tag mapping\n");
    printf("         -v2x convert from vertical to XML format\n");
    printf("      -v2xmap convert from vertical to XML format with POS tag mapping\n");
    printf("      -nwulem NWU conversion with supp file\n");
    printf("         -cup CUP conversion passing all tags through, stripping ref no and replacing supp refs in vertical version\n");
    printf("         -p2h convert from parsed to horizontal format\n");
    printf("         -p2v convert from parsed to vertical format\n");
    printf("         -b2h convert from BNC to horizontal format\n");
    printf("         -b2v convert from BNC to vertical format\n");
    puts("         -s2h convert from semtag output to horizontal format");
    puts("     -s2hsupp convert from semtag output to horizontal format with supp file");
    puts("    -s2hstrip convert from semtag output to horizontal format with stripped tags");
    puts("  -s2hstripos convert from semtag output to horizontal format with stripped tags and reduced POS set");
    puts("         -s2k convert from semtag output to xkwic format");
    puts("     -s2ksupp convert from semtag output to xkwic format with supp file");
    puts("         -s2x convert from semtag output to XML format");
    puts("      -s2xmap convert from semtag output to XML format with POS tag mapping");
    puts("         -l2t convert from lemma output to TreeTagger format");
    puts("       -nosos don't show start of sentence markers and NULL tags in output");
    puts("     -sentnum show start of sentence ref numbers in output");
    puts("        -para expect parallel vertical input and display different CLAWS tags");
    puts("        -rare remove CLAWS rarity markers (only in vertical to horizontal conv)");
    puts("     -fletter in horizontal output display only first letter of CLAWS tag");
    puts("  -noeolnsupp replace end-of-line characters in supp file with spaces");
    exit(1);

}	/* end of help */


int is_punc(syntag)
/* Test whether the parameter is a punctuation symbol. */
char	syntag[SYNSIZE];
{
int	i = 0, found = FALSE;

    /* Test whether item is entry in list of punctuation. */
    while ((!found) && (i < MAX_PUNC))
        {
        if (strcmp(syntag, punc[i].item) == 0)
            found = TRUE;
        i++;
        }
    return(found);

}	/* end of is_punc */


int member(c, s1)
/* determine whether the string contains the character */
char	c, *s1;
{
int	i, ln;
	
    ln = strlen(s1);
    for (i = 0; i < ln; i++)
        {
        if (c == s1[i])
            return(TRUE);
        else if (s1[i] == ENDOFSTR)
            return(FALSE);
        }
    return(FALSE);

}	/* end of member */


int tenpow(power)
/* returns ten to a given power using recursive calls */
int	power;
{
    if (power == 0)
        return(1);
    else
        return(BASE*tenpow(power-1));

}	/* end of tenpow */


void itostr(x, str, digits)
/* convert integer x to string and append it to str */
int	x, digits;
char	str[MARKSIZE];
{
int	power, quot, rem, pos = 0, temp;

    rem = x;
    temp = strlen(str);
    /* how many digits to show .... */
    pos = digits;
    power = tenpow(pos-1);
    while (pos > 0)
        {
        quot = rem / power;
        rem = rem % power;
        str[temp++] = (char) quot + '0';
        power = power / BASE;
        pos--;
        }
    str[temp] = ENDOFSTR;
        
}	/* end of itostr */


void separate_tag(s1, i)
/* after reading in string separate syntag from word */
int	i;
char	*s1;
{
int	loop, t, x, ln;
char	syntag[SYNSIZE], tempw[100], temps[100];

    syntag[0] = ENDOFSTR;
    t = strlen(s1);
    for (loop = 0; loop <= t; loop++)
        if (s1[loop] == '_')
            {
            x = ++loop;
            for (loop = x; loop <= t; loop++)
                syntag[loop-x] = s1[loop];
            syntag[loop-x] = ENDOFSTR;
            s1[x-1] = ENDOFSTR;
            break;
            }

    ln = strlen(syntag);
    strcpy(buffer[i].word, s1);
    strcpy(buffer[i].syntag, syntag);

}	/* end of separate_tag */
