/* assume vertical input file format (from CLAWS4):

ref_number  word                             decision_code syntax_tags

0000007 040 tuckoo                                      06 NN1
0000007 041 ....                                        03 ...
0000008 001 ----------------------------------------------------
0000008 010 His                                         03 [APP$/100] PP$@/0

also assume REFTAGDIST chars between end of ref-no and syntax-tags

Sometimes there are no tags:

0051312 040 for                                         93 [IF/100] CS%/0
0051312 050 this                                        93 [DD1/98] RG%/2
0051312 060 ----ing
0051313 010 family                                      93 NN1
0051313 011 .                                           03 .

for anonymisation. This gets a warning and tagged NOTAG.

*/

#include <stdio.h>
#include "util.h"

void init_claws_line(ref, word, code, tags)
/* initialise strings and tag list */
char	*ref,			/* reference number */
	*word,			/* word!! */
	*code;			/* decision code */
claws_line_tags_ent	*tags;	/* linked list of CLAWS tags */
{
claws_line_tags_ent	*p, *new;

    /* initialise */
    strcpy(ref, "");
    strcpy(word, "");
    strcpy(code, "");
    /* initialise tag list */
    p = tags;
    while (p != NULL)
        {
        free(p->tag);
        free(p->pct);
        new = p;
        p = p->next;
        free(new);
        }

}	/* end of init_claws_line */


void free_claws_line_tags(tags)
/* free tag list */
claws_line_tags_ent	*tags;	/* linked list of CLAWS tags */
{
claws_line_tags_ent	*p, *new;

    /* initialise tag list */
    p = tags;
    while (p != NULL)
        {
        free(p->tag);
        free(p->pct);
        new = p;
        p = p->next;
        free(new);
        }

}	/* end of free_claws_line_tags */


int read_claws_line(fp, ref, word, code, tags)
/* THIS PROCEDURE IS DEPRECATED - REPLACED BY READ_CLAWS_LINE2 */
/* read one line at a time of CLAWS output */
/* return TRUE at end of file or for a format error */
FILE	*fp;			/* file pointer to read from */
char	*ref,			/* reference number */
	*word,			/* word!! */
	*code;			/* decision code */
claws_line_tags_ent	**tags;	/* linked list of CLAWS tags */
{
int	wdln, i;
int	no_tag_list;		/* flag to indicate no list of tags */
char	c;
char	tempref1[WORDSIZE], tempref2[WORDSIZE],
	tempsyn[WORDSIZE], temppct[WORDSIZE];
claws_line_tags_ent	*new, *p, *tagend;

    /* read CLAWS reference number */
    if (fscanf(fp, "%s", tempref1) == EOF)
        {
/*        fprintf(stderr, "CLAWS Format error 1\n");*/
        return(TRUE);
        }
    if (fscanf(fp, "%s", tempref2) == EOF)
        {
        non_fatal_error("CLAWS Format error (early end of file)", tempref1);
        return(TRUE);
        }
    sprintf(ref, "%s %s", tempref1, tempref2);
    /* format check - length should be 11 chars */
    if (strlen(ref) != 11)
        {
        non_fatal_error("CLAWS Format error (reference number error?)", ref);
        return(TRUE);
        }

    /* read word */
    fscanf(fp, "%s", word);
    wdln = strlen(word);

/****TODO*******/
            /* if word starts with '*<' make sure all of it is read in */
            if ((strncmp(word, "*<", 2) == 0)
            && (word[wdln-1] != '*')
            && (word[wdln-2] != '>'))
                rep_err("help me please - i am stuck!");
/*                {
                found = FALSE;
                c = getc(ifp);
                while (!found)
                    {
                    d = getc(ifp);
                    if ((c == '>') && (d == '*'))
                        {
                        found = TRUE;
                        temporig[sent[cur].owln++] = c;
                        temporig[sent[cur].owln++] = d;
                        temporig[sent[cur].owln] = EOS;
                        }
                    else
                        {
*/                        /* make sure we don't go too far */
/*                        if (sent[cur].owln < REFTAGDIST-4)
                            {
                            temporig[sent[cur].owln++] = c;
                            c = d;
                            }
                        else
                            {
                            found = TRUE;
                            temporig[sent[cur].owln++] = c;
                            temporig[sent[cur].owln++] = d;
                            temporig[sent[cur].owln] = EOS;
                            }
                        }
                    }
                }
*/

    /* if this isn't a sentence break read tags */
    /* sentence breaks are undesirably long */
    if (!strcmp(SBREAKWORD, word))
        {
        strcpy(word, SBREAKTAG);
        /* copy SBREAKTAG to taglist */
        /* allocate space for tag */
        new = NEW_STRUCT(claws_line_tags_ent);
        new->tag = STRING(SBREAKTAG);
        new->pct = STRING("");
        new->next = NULL;
        *tags = new;
        /* read any more gumpf on the end of the line */
        c = getc(fp);
        while (c != EOLN)
            {
            if (c == EOF) return(TRUE);
            c = getc(fp);
            }
        }
    else
        {
        no_tag_list = FALSE;
        wdln = strlen(word);
        /* read all chars up to decision code: skip < > and other junk */
        for (i = 0; i <= (REFTAGDIST - wdln - 3); i++)
            {
            c = getc(fp);
            /* some lines have no tag list */
            if (c == EOLN)
                {
                no_tag_list = TRUE;
                break;
                }
            }

                /* get rid of spaces at end - have to do this here to make owln correct */
/*                sent[cur].owln--;
                while (temporig[sent[cur].owln] == ' ')
                    sent[cur].owln--;
                temporig[++sent[cur].owln] = EOS;
*/

        if (no_tag_list)
            {
            non_fatal_error("CLAWS Format (no tag list)", ref);
            strcpy(code, "00");
            /* allocate space for rubbish tag */
            new = NEW_STRUCT(claws_line_tags_ent);
            new->tag = STRING("NOTAG");
            new->pct = STRING("");
            new->next = NULL;
            *tags = new;
            }
        else
            {
            /* read decision code */
            fscanf(fp, "%s", code);
    
            /* read tag list */
            *tags = NULL;
            tagend = *tags;
            c = getc(fp); c = getc(fp);
            if (c == '[') c = getc(fp);
            while (c != EOF)
                {
                /* read tag */
                i = 0;
                while (!member(c, " ]/\n"))
                    {
                    tempsyn[i++] = c;
                    c = getc(fp);
                    }
                tempsyn[i] = EOS;
                i = 0;
                if (c == '/')
                    {
                    /* read percentage */
                    c = getc(fp);
                    while (!member(c, " ]\n"))
                        {
                        temppct[i++] = c;
                        c = getc(fp);
                        }
                    }
                temppct[i] = ENDOFSTR;
                /* read space after bracketed tag */
                if (c == ']')
                    {
                    /* make sure we don't lose last bracket from NPEXT */
                    if (strstr(tempsyn, "Np"))
                        strcat(tempsyn, "]");
                    c = getc(fp);
                    }

                /* allocate space for new tag */
                new = NEW_STRUCT(claws_line_tags_ent);
                new->tag = STRING(tempsyn);
                new->pct = STRING(temppct);
                new->next = NULL;
                /* if list is empty attach to start */
                if (*tags == NULL)
                    {
                    *tags = new;
                    tagend = *tags;
                    }
                else
                    {
                    /* attach to end of list */
                    tagend->next = new;
                    tagend = tagend->next;
                    }
                if (c == EOLN) break;
                c = getc(fp);
                while (c == ' ') c = getc(fp);
                }
            }
        }

/*    while (c != EOLN)
        {
        if (c == EOF)
            {
            return(TRUE);
            }
        c = getc(fp);
        }
*/
    /* infile is not empty */
    return(FALSE);
    
}	/* end of read_claws_line */


int read_claws_line2(fp, ref, word, code, tags, split)
/* read one line at a time of CLAWS output */
/* this version passes back the CLAWS enclitic markers */
/* return TRUE at end of file or for a format error */
FILE	*fp;			/* file pointer to read from */
char	*ref,			/* reference number */
	*word,			/* word!! */
	*code,			/* decision code */
	*split;			/* returned CLAWS split-word marker */
claws_line_tags_ent	**tags;	/* linked list of CLAWS tags */
{
int	wdln, i;
int	no_tag_list;		/* flag to indicate no list of tags */
char	c;
char	tempref1[WORDSIZE], tempref2[WORDSIZE],
	tempsyn[WORDSIZE], temppct[WORDSIZE];
claws_line_tags_ent	*new, *p, *tagend;

    strcpy(split, " ");
    
    /* read CLAWS reference number */
    if (fscanf(fp, "%s", tempref1) == EOF)
        {
/*        fprintf(stderr, "CLAWS Format error 1\n");*/
        return(TRUE);
        }
    if (fscanf(fp, "%s", tempref2) == EOF)
        {
        non_fatal_error("CLAWS Format error (early end of file)", tempref1);
        return(TRUE);
        }
    sprintf(ref, "%s %s", tempref1, tempref2);
    /* format check - length should be 11 chars */
    if (strlen(ref) != 11)
        {
        non_fatal_error("CLAWS Format error (reference number error?)", ref);
        return(TRUE);
        }

    /* read word */
    fscanf(fp, "%s", word);
    wdln = strlen(word);

/****TODO*******/
            /* if word starts with '*<' make sure all of it is read in */
            if ((strncmp(word, "*<", 2) == 0)
            && (word[wdln-1] != '*')
            && (word[wdln-2] != '>'))
                rep_err("help me please - i am stuck!");
/*                {
                found = FALSE;
                c = getc(ifp);
                while (!found)
                    {
                    d = getc(ifp);
                    if ((c == '>') && (d == '*'))
                        {
                        found = TRUE;
                        temporig[sent[cur].owln++] = c;
                        temporig[sent[cur].owln++] = d;
                        temporig[sent[cur].owln] = EOS;
                        }
                    else
                        {
*/                        /* make sure we don't go too far */
/*                        if (sent[cur].owln < REFTAGDIST-4)
                            {
                            temporig[sent[cur].owln++] = c;
                            c = d;
                            }
                        else
                            {
                            found = TRUE;
                            temporig[sent[cur].owln++] = c;
                            temporig[sent[cur].owln++] = d;
                            temporig[sent[cur].owln] = EOS;
                            }
                        }
                    }
                }
*/

    /* if this isn't a sentence break read tags */
    /* sentence breaks are undesirably long */
    if (!strcmp(SBREAKWORD, word))
        {
        strcpy(word, SBREAKTAG);
        /* copy SBREAKTAG to taglist */
        /* allocate space for tag */
        new = NEW_STRUCT(claws_line_tags_ent);
        new->tag = STRING(SBREAKTAG);
        new->pct = STRING("");
        new->next = NULL;
        *tags = new;
        /* read any more gumpf on the end of the line */
        c = getc(fp);
        while (c != EOLN)
            {
            if (c == EOF) return(TRUE);
            c = getc(fp);
            }
        }
    else
        {
        no_tag_list = FALSE;
        wdln = strlen(word);
        /* read all chars up to decision code: skip < > and other junk */
        for (i = 0; i <= (REFTAGDIST - wdln - 3); i++)
            {
            c = getc(fp);
            /* fiddle for CUP which passes this back */
            if (c == '<' || c == '>')
                {
                split[0] = c;
                }
            /* some lines have no tag list */
            else if (c == EOLN)
                {
                no_tag_list = TRUE;
                break;
                }
            }

                /* get rid of spaces at end - have to do this here to make owln correct */
/*                sent[cur].owln--;
                while (temporig[sent[cur].owln] == ' ')
                    sent[cur].owln--;
                temporig[++sent[cur].owln] = EOS;
*/

        if (no_tag_list)
            {
            non_fatal_error("CLAWS Format (no tag list)", ref);
            strcpy(code, "00");
            /* allocate space for rubbish tag */
            new = NEW_STRUCT(claws_line_tags_ent);
            new->tag = STRING("NOTAG");
            new->pct = STRING("");
            new->next = NULL;
            *tags = new;
            }
        else
            {
            /* read decision code */
            fscanf(fp, "%s", code);
    
            /* read tag list */
            *tags = NULL;
            tagend = *tags;
            c = getc(fp); c = getc(fp);
            if (c == '[') c = getc(fp);
            while (c != EOF)
                {
                /* read tag */
                i = 0;
                while (!member(c, " ]/\n"))
                    {
                    tempsyn[i++] = c;
                    c = getc(fp);
                    }
                tempsyn[i] = EOS;
                i = 0;
                if (c == '/')
                    {
                    /* read percentage */
                    c = getc(fp);
                    while (!member(c, " ]\n"))
                        {
                        temppct[i++] = c;
                        c = getc(fp);
                        }
                    }
                temppct[i] = ENDOFSTR;
                /* read space after bracketed tag */
                if (c == ']')
                    {
                    /* make sure we don't lose last bracket from NPEXT */
                    if (strstr(tempsyn, "Np"))
                        strcat(tempsyn, "]");
                    c = getc(fp);
                    }
    
                /* allocate space for new tag */
                new = NEW_STRUCT(claws_line_tags_ent);
                new->tag = STRING(tempsyn);
                new->pct = STRING(temppct);
                new->next = NULL;
                /* if list is empty attach to start */
                if (*tags == NULL)
                    {
                    *tags = new;
                    tagend = *tags;
                    }
                else
                    {
                    /* attach to end of list */
                    tagend->next = new;
                    tagend = tagend->next;
                    }
                if (c == EOLN) break;
                c = getc(fp);
                while (c == ' ') c = getc(fp);
                }
            }
        }

/*    while (c != EOLN)
        {
        if (c == EOF)
            {
            return(TRUE);
            }
        c = getc(fp);
        }
*/
    /* infile is not empty */
    return(FALSE);
    
}	/* end of read_claws_line2 */

