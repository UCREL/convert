#include "convert.h"

int read_map(mapfn)
/* read mapping file in filename provided and return number of entries */
char	*mapfn;
{
FILE	*mfp;
int	count = 0, pos, pos2;
char	line[LGWORDSIZE];

    if ((mfp = fopen(mapfn, "r")) == NULL)
        {
	rep_err("Mapping file not found.");
	}
/*    fscanf(mfp, "%s\t%s", mapping[count].from, mapping[count].to);*/
    fgets(line, LGWORDSIZE, mfp);
    pos = 0; pos2 = 0;
    while (line[pos] != '\t' && line[pos] != '\n') { mapping[count].from[pos] = line[pos]; pos++; }
    mapping[count].from[pos] = EOS;
    pos++;
    while (line[pos] != '\n' && line[pos] != EOS) { mapping[count].to[pos2] = line[pos]; pos++; pos2++; }
    mapping[count].to[pos2] = EOS;
/*    printf("%s\t%s\n", mapping[count].from, mapping[count].to);*/

    while (!feof(mfp))
	{
	count++;
/*	fscanf(mfp, "%s\t%s", mapping[count].from, mapping[count].to);*/
        fgets(line, LGWORDSIZE, mfp);
        pos = 0; pos2 = 0;
        while (line[pos] != '\t' && line[pos] != '\n') { mapping[count].from[pos] = line[pos]; pos++; }
        mapping[count].from[pos] = EOS;
        pos++;
        while (line[pos] != '\n' && line[pos] != EOS) { mapping[count].to[pos2] = line[pos]; pos++; pos2++; }
        mapping[count].to[pos2] = EOS;
/*	printf("%s\t%s\n", mapping[count].from, mapping[count].to);*/
	}
    printf("%d entries found in mapping file.\n", count);
    return(count - 1);


}	/* end of read_map */


int read_map3(mapfn)
/* read mapping file in filename provided and return number of entries */
/* this version assumes the first column contains a type for Varding */
char	*mapfn;
{
FILE	*mfp;
int	count = 0, pos, pos1, pos2, pos3;
char	line[LGWORDSIZE];

    if ((mfp = fopen(mapfn, "r")) == NULL)
        {
	rep_err("Mapping file not found.");
	}
/*    fscanf(mfp, "%s\t%s", mapping[count].from, mapping[count].to);*/
    fgets(line, LGWORDSIZE, mfp);
    pos = 0; pos1 = 0; pos2 = 0; pos3 = 0;
    while (line[pos] != '\t' && line[pos] != '\n') { mapping[count].type[pos3] = line[pos]; pos++; pos3++;}
    mapping[count].type[pos3] = EOS;
    pos++;
    while (line[pos] != '\t' && line[pos] != '\n') { mapping[count].from[pos1] = line[pos]; pos++; pos1++;}
    mapping[count].from[pos1] = EOS;
    pos++;
    while (line[pos] != '\n' && line[pos] != EOS) { mapping[count].to[pos2] = line[pos]; pos++; pos2++; }
    mapping[count].to[pos2] = EOS;
/*    printf("%s\t%s\n", mapping[count].from, mapping[count].to);*/

    while (!feof(mfp))
	{
	count++;
/*	fscanf(mfp, "%s\t%s", mapping[count].from, mapping[count].to);*/
        fgets(line, LGWORDSIZE, mfp);
        pos = 0; pos1 = 0; pos2 = 0; pos3 = 0;
        while (line[pos] != '\t' && line[pos] != '\n') { mapping[count].type[pos3] = line[pos]; pos++; pos3++;}
        mapping[count].type[pos3] = EOS;
        pos++;
        while (line[pos] != '\t' && line[pos] != '\n') { mapping[count].from[pos1] = line[pos]; pos++; pos1++;}
        mapping[count].from[pos1] = EOS;
        pos++;
        while (line[pos] != '\n' && line[pos] != EOS) { mapping[count].to[pos2] = line[pos]; pos++; pos2++; }
        mapping[count].to[pos2] = EOS;
/*	printf("%s\t%s\n", mapping[count].from, mapping[count].to);*/
	}
    printf("%d entries found in mapping file.\n", count);
    return(count - 1);


}	/* end of read_map3 */


void map_tags()
/* applies mapping to sentence */
{
int	ln;
int     cur, mapos;

    for (cur=0; cur <= length; cur++)
        {
        /* delete @ and % markers on tags */
	ln = buffer[cur].sxln;
        if (strchr("@%", buffer[cur].syntag[ln-1]))
            {
            buffer[cur].syntag[ln-1] = EOS;
            buffer[cur].sxln--;
            }
	/* delete CLAWS ditto tag markers */
	ln = buffer[cur].sxln;
	if (isdigit(buffer[cur].syntag[ln-1]) && isdigit(buffer[cur].syntag[ln-2]))
            {
            buffer[cur].syntag[ln-2] = EOS;
            buffer[cur].sxln =- 2;
            }
	/* look up pos tag in mapping table */
	mapos = 0;
	while (strcmp(mapping[mapos].from, buffer[cur].syntag) && mapos <= numaps)
		{
		mapos++;
		}
	if (mapos > numaps)
		{
		/* not found */
		/* strcat(buffer[cur].syntag, "NOTFOUND");*/
		fprintf(stderr, "Tag %s not found.\n", buffer[cur].syntag);
		}
	else
		{
		strcpy(buffer[cur].syntag, mapping[mapos].to);
		}
	/* for punctuation lemma should be the same as the 'word' */
	if (!strcmp(buffer[cur].semtag, "PUNC"))
		{
		strcpy(buffer[cur].semtag, buffer[cur].word);
		}
        }
    /* mark end of sentence differently */
    if (length != 0 && !semantic_xml_map && !vert_xml_map)
	strcpy(buffer[length-1].syntag, "SENT");

}	/* end of map tags */


void map_word(word)
/* applies mapping to a word */
char	*word;
{
int	ln;
int     cur, mapos;
char	nword[LGWORDSIZE];

	/* look up word in mapping table */
	/* added ignoring case Dec 04 */
	mapos = 0;
	while (strcasecmp(mapping[mapos].from, word) && mapos <= numaps)
		{
		mapos++;
		}
	if (mapos > numaps)
		{
		/* not found */
		/* strcat(buffer[cur].syntag, "NOTFOUND");*/
		}
	else
		{
		sprintf(nword, "<reg %s=\"%s\">%s", mapping[mapos].type, word, mapping[mapos].to);
		/* printf("Found: '%s' '%s'\n", word, mapping[mapos].to); */
		strcpy(word, nword);
		}

}	/* end of map_word */



