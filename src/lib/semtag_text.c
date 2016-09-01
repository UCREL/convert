/* assume SEMTAG output format */

/*
0000001 001  NULL    **83;0;person            
0000003 001  NULL    <question>               
0000003 002  -----   -----                    
0000003 010  CS      If                       Z7 
0000003 020  PPIS2   we                       Z8 
0000003 030  VM      could                    A7+ X9.1+ 
0000003 040  VVI     start                    T2+[i1.2.1 T2+ E5- E3- 
0000003 050  RP      off                      T2+[i1.2.2 M6 K1 A1.1.1 
0000003 060  II      by                       Z5 
0000003 070  VVG     talking                  Q2.1 
0000003 080  II      about                    Z5 
0000003 090  DDQ     what                     Z8 Z5 
0000003 091  VBZ     's                       A3+ Z5 
0000003 100  II      on                       Z5 
0000003 110  APPGE   your                     Z8 
0000003 120  NN1     mind                     X2 
*/

#include <stdio.h>
#include "util.h"

int get_semtag_next_line(ifp, ref, word, syn, sem, parse, extract_flag)
/* read one item (one line) from a SEMTAG output file */
FILE	*ifp;
char	*ref, *word, *syn, *sem,
	*parse;			/* NP parsing brackets */
int	extract_flag;		/* whether to extract one or all sem tags */
{
char	c, d, tempref[REFSIZE],
	rubbish[LGWORDSIZE],
	tempsem[LGWORDSIZE];
int	found, wdln,
	infile_empty = FALSE;

    /* assume input file format : ref_no syntax_tag word semantic_tags */
    /* also assume REFTAGDIST chars between end of ref-no and syntax-tags */

    sem[0] = EOS; parse[0] = EOS;

    if (fscanf(ifp, "%s", ref) != EOF)
        {
        fscanf(ifp, "%s %s %s", tempref, syn, word);
        wdln = strlen(word);
        strcat(ref, " ");
        strcat(ref, tempref);

        /* temporary fiddle to read lemmas */
/*        fscanf(ifp, " %s", rubbish);*/
        
        /* if word starts with *< make sure all of it is read in */
        if ((word[0] == '*') && (word[1] == '<')
        && (word[wdln-1] != '*')
        && (word[wdln-2] != '>'))
            {
            found = FALSE;
            c = getc(ifp);
            while (!found)
                {
                if (c == EOLN)
                    {
                    ungetc(c, ifp);
                    found = TRUE;
                    word[wdln] = EOS;
                    break;
                    }
                d = getc(ifp);
                if ((c == '>') && (d == '*'))
                    {
                    found = TRUE;
                    word[wdln++] = c;
                    word[wdln++] = d;
                    word[wdln] = EOS;
                    }
                else
                    {
                    /* make sure we don't go too far */
                    if (wdln < REFTAGDIST-4)
                        {
                        word[wdln++] = c;
                        c = d;
                        }
                    else
                        {
                        found = TRUE;
                        word[wdln++] = c;
                        word[wdln++] = d;
                        word[wdln] = EOS;
                        }
                    }
                }
            }

        wdln = strlen(word);
        /* get rid of spaces at end - have to do this here to make wdln correct */
        wdln--;
        while (word[wdln] == ' ')
            wdln--;
        word[++wdln] = EOS;

        c = getc(ifp);
        while (c != EOLN)
            {
            while (c == SPACE)
                c = getc(ifp);
            if (c != EOLN)
                {
                ungetc(c, ifp);
                fscanf(ifp, "%s", tempsem);
                /* record NP parsing */
                if (strstr(tempsem, "Np"))
                    strcat(parse, tempsem);
                /* record the first semtag only */
                else if (sem[0] == EOS)
		    {
/*		    printf("Word: %s\nFirst: %s\n", word, sem);*/
                    strcpy(sem, tempsem);
		    }
		/* record and conflate all tags */
		else if (extract_flag == EXTRACT_CONFLATED_TAGS)
			{
			strcat(sem, ":");
			strcat(sem, tempsem);
/*			printf("Concat: %s\n", sem);*/
			}

/*                break;*/
/* TODO = record multiple semantic tags 
Use SemTagListLink in ~/include/util.h */
/*                if (i < MAX_TAGSEM)
                    {
                    strcpy(sent[cur].semtag[i].data, temp);
                    sent[cur].smno++;
                    }
                else
                    {
                    ERROR - to many sem tags. 
                    rep_err("too many sem tags"); 
                    }
                ++i;
*/
                c = getc(ifp);
                }
            }
/*        c = getc(ifp);*/
        /* read the rest of the line */
        while (c != EOLN)
            {
            if (c == EOF)
                {
                infile_empty = TRUE;
                break;
                }
            c = getc(ifp);
            }

        /* if no semantic tags assume punctuation */
        if (sem[0] == EOS) strcpy(sem, "PUNC");
        }
    else
        {
        /* rep_err("*** at EOF ***"); */
        infile_empty = TRUE;
        }
    return(infile_empty);

}	/* end of get_semtag_next_line */


linkent *semtag_strip_marker(s1, idcn, idno, idpo,
    slash_prefer_first, strip_gender_marker)
/* remove markers from end of semantic tags (might be more than one)
and return integers which reflect info encoded in the removed part */
/* for example in semantic tag 'T2+[i1.2.1' we pass back 'T2+' in s1,
idcn = 1, idno = 2; idpo = 1 and return a link entry structure */
char	*s1;
int	*idcn, *idno, *idpo;
int	slash_prefer_first,	/* flag to chop second tag from slash tag */
	strip_gender_marker;	/* flag to strip lower case letters from semtags */
{
int	x, smln, endpt, templn=NIL;
char	temp[LGWORDSIZE];
linkent *new, *end, *link;

    link = NULL;
    end = link;
    *idcn = *idno = *idpo = 0;
    endpt = smln = strlen(s1);
    for (x = 0; x <= smln; x++)
        {
        /* start of links and idioms shown by '[' marker */
        if (s1[x] == SEMTAG_LINK_MARKER)
            {
            /* record pointer to end of semantic tag */
            if (x < endpt) endpt = x;
            x++;
            /* semantic idiom shown by 'i' */
            if (s1[x] == 'i')
                {
                templn = NIL;
                temp[++templn] = s1[++x];
                while (temp[templn] != SEMTAG_LINK_SEPRT)
                    temp[++templn] = s1[++x];
                temp[templn] = EOS;
                *idcn = atoi(temp);
                templn = NIL;
                temp[++templn] = s1[++x];
                while (temp[templn] != SEMTAG_LINK_SEPRT)
                    temp[++templn] = s1[++x];
                temp[templn] = EOS;
                *idno = atoi(temp);
                templn = NIL;
                temp[++templn] = s1[++x];
                while ((temp[templn] != EOS) && (temp[templn] != SEMTAG_LINK_MARKER))
                    temp[++templn] = s1[++x];
                temp[templn] = EOS;
                *idpo = atoi(temp);
                x--;
                }
            /* mash link from MATRIX */
            else if (strchr("mashd", s1[x]))
                {
                templn = NIL;
                new = NEW_STRUCT(linkent);
                new->linktype = s1[x];
                temp[++templn] = s1[++x];
                while (temp[templn] != SEMTAG_LINK_SEPRT)
                    temp[++templn] = s1[++x];
                temp[templn] = EOS;
                new->linkcn = atoi(temp);
                templn = NIL;
                temp[++templn] = s1[++x];
                while (temp[templn] != SEMTAG_LINK_SEPRT)
                    temp[++templn] = s1[++x];
                temp[templn] = EOS;
                new->linkno = atoi(temp);
                templn = NIL;
                temp[++templn] = s1[++x];
                while ((temp[templn] != EOS) && (temp[templn] != SEMTAG_LINK_MARKER))
                    temp[++templn] = s1[++x];
                temp[templn] = EOS;
                new->linkpo = atoi(temp);
                new->next = NULL;
                /* if list is empty attach to start */
                if (link == NULL)
                    {
                    link = new;
                    end = link;
                    }
                else
                    {
                    /* attach to end of list */
                    end->next = new;
                    end = end->next;
                    }
                x--;
                }

            }
        /* don't include gender markers */
        else if (islower(s1[x]) && strip_gender_marker)
            {
            /* record pointer to end of semantic tag */
            if (x < endpt) endpt = x;
            }
        /* cut slash tags back to the first part if required */
        else if (slash_prefer_first && s1[x] == '/')
            {
            /* record pointer to end of semantic tag */
            if (x < endpt) endpt = x;
            }
        /* remove proximity weightings in {} from end of tag */
        else if (s1[x] == '{')
            {
            /* record pointer to end of semantic tag */
            if (x < endpt) endpt = x;
            }
        }

    s1[endpt] = EOS;
    return(link);

}	/* end of semtag_strip_marker */
