#include "convert.h"

int read_sent_horz(fp)
/* read in one sentence at a time */
FILE *fp;
{
int	current_entry = 0,		/* current pos in sentence */ 
	x, end_of_sent = FALSE;
char	c;

    c = getc(fp);
    while (member(c, "^ \n"))
        {
        /* search for start of sentence */
        if (c == EOF)
            {
            infile_empty = TRUE;
            return(0);
            }
        c = getc(fp);
        }
/*    c = getc(fp);
*/
    while (!end_of_sent)
        {
        init_bufent(current_entry);
        while (member(c," \n"))
            {
            c = getc(fp);
            /* skip spaces and eolns */
            if (c == EOF)
                {
/*                fprintf(stderr,"*** unexpected EOF ***\n");
                exit(1);
*/
		current_entry--;
		return(current_entry);
                }
            }
        if (c == '^')
            {
            /* start of next sentence */
            end_of_sent = TRUE;
            ungetc(c,fp);
            return(current_entry-1);
            }
        x = 0;
        while (c != '_')
            {
            /* read in word */
            buffer[current_entry].word[x++] = c;
            c = getc(fp);
            if (c == EOF)
                {
/*                fprintf(stderr,"*** unexpected EOF ***\n");
*/                exit(0);
                }
            }
        buffer[current_entry].word[x] = ENDOFSTR;
        c = getc(fp);
        x = 0;
        while (!member(c, " \n"))
            {
            /* read in syntax tag */
            buffer[current_entry].syntag[x++] = c;
            c = getc(fp);
            if (c == EOF)
                {
/*                fprintf(stderr, "*** unexpected EOF ***\n");
                exit(0);
*/                }
            }
        buffer[current_entry].syntag[x] = ENDOFSTR;
        if (member(buffer[current_entry].word[0], ".")
        || member(buffer[current_entry].word[0], "!")
        || member(buffer[current_entry].word[0], "?"))
            {
            /* stop at end of sentence */
            end_of_sent = TRUE;
            }
        else
            {
            /* or read next word_tag pair */
            current_entry++;
            if (current_entry == BUFFERSIZE)
                {
                current_entry--;
                non_fatal_error("sentence too long at ", buffer[current_entry].word);
                return(current_entry);
                }
            }
        }

    /* return length of sentence */
    return(current_entry);

}	/* end of read_sent_horz */


void write_sent_horz(fp, length, no_sos, show_sent_num)
/* writes out current sentence to output file with associated markers */
FILE	*fp;
int	length;				/* length of sentence */
int	no_sos;
int	show_sent_num;
{
int	offset = 0,			/* length of current line */
	x, temp, addlen;
char	tempsupp[100000];

    if (!no_sos)
        {
        fprintf(fp,"^ ");
        offset += 2;
        }
    for (x = 0; x <= length; x++)
        {
        /* check both POS and word fields in case POS has been mapped */
        if ((strcmp(SBREAKTAG, buffer[x].syntag) == 0) || (strcmp(SBREAKTAG, buffer[x].word) == 0))
            {
            if (show_sent_num)
                {
                addlen = strlen(buffer[x].ref) + 1;
                offset = addlen;
                fprintf(fp, "\n\n%s ", buffer[x].ref);
                }
            continue;
            }
        /* check for supp file references */
        if (is_supp_ref(buffer[x].word) && (vert_horz_supp || vert_horz_supp_map))
            {
            get_string_from_supp(buffer[x].word, tempsupp);
            if (fletter)
                addlen = strlen(tempsupp) + 3;
            else if (!strcmp(buffer[x].syntag, "NULL") && no_sos)
                addlen = strlen(tempsupp) + 2;
            else
                addlen = strlen(tempsupp) + 2 + strlen(buffer[x].syntag);
            }
        else
            {
            if (fletter)
                addlen = strlen(buffer[x].word) + 3;
            else if (!strcmp(buffer[x].syntag, "NULL") && no_sos)
                addlen = strlen(buffer[x].word) + 2;
            else
                addlen = strlen(buffer[x].word) + 2 + strlen(buffer[x].syntag);
            }
        if (offset+addlen >= LINELENGTH)
            {
            fprintf(fp,"\n");
            /* eoln reached */
            offset = addlen;
            }
        else
            {
            offset += addlen;
            }

        if (fletter)
            {
            if (is_supp_ref(buffer[x].word) && (vert_horz_supp || vert_horz_supp_map))
                fprintf(fp, "%s_%c", tempsupp, buffer[x].syntag[0]);
            else
                fprintf(fp, "%s_%c", buffer[x].word, buffer[x].syntag[0]);
            }
        else
            {
            if (!(is_supp_ref(buffer[x].word) && (vert_horz_supp || vert_horz_supp_map)))
                strcpy(tempsupp, buffer[x].word);

            if (!strcmp(buffer[x].syntag, "NULL") && no_sos)
                fprintf(fp, "%s", tempsupp);
            else
                fprintf(fp, "%s_%s", tempsupp, buffer[x].syntag);

/*            else
                fprintf(fp, "%s_%s", buffer[x].word, buffer[x].syntag);
*/
            }
        fprintf(fp, " ");
        }
    if (!no_sos && !show_sent_num)
        fprintf(fp, "\n\n");
    else
        fprintf(fp, "\n");

}	/* end of write_sent_horz */


void write_semsent_horz(fp, length, no_sos, show_sent_num)
/* writes out current sentence to output file with associated markers */
FILE	*fp;
int	length;				/* length of sentence */
int	no_sos;
int	show_sent_num;
{
int	offset = 0,			/* length of current line */
	x, temp, addlen;
char	tempsupp[100000];

    if (!no_sos)
        {
        fprintf(fp,"^ ");
        offset += 2;
        }
    for (x = 0; x <= length; x++)
        {
        if (strcmp(SBREAKTAG, buffer[x].word) == 0)
            {
            if (show_sent_num)
                {
                addlen = strlen(buffer[x].ref) + 1;
                offset = addlen;
                fprintf(fp, "\n\n%s ", buffer[x].ref);
                }
            continue;
            }
        /* check for supp file references */
        if (is_supp_ref(buffer[x].word) && semantic_horz_supp)
            {
            get_string_from_supp(buffer[x].word, tempsupp);
            if (fletter)
                addlen = strlen(tempsupp) + 3;
            else
                addlen = strlen(tempsupp) + 2 + strlen(buffer[x].semtag);
            }
        else
            {
            if (fletter)
                addlen = strlen(buffer[x].word) + 3;
            else
                addlen = strlen(buffer[x].word) + 2 + strlen(buffer[x].semtag);
            }
        if (offset+addlen >= LINELENGTH)
            {
            fprintf(fp,"\n");
            /* eoln reached */
            offset = addlen;
            }
        else
            {
            offset += addlen;
            }

        if (fletter)
            {
            if (is_supp_ref(buffer[x].word) && semantic_horz_supp)
                fprintf(fp, "%s_%c", tempsupp, buffer[x].semtag[0]);
            else
                fprintf(fp, "%s_%c", buffer[x].word, buffer[x].semtag[0]);
            }
        else
            {
            if (is_supp_ref(buffer[x].word) && semantic_horz_supp)
                fprintf(fp, "%s_%s", tempsupp, buffer[x].semtag);
            else
                fprintf(fp, "%s_%s", buffer[x].word, buffer[x].semtag);
            }
        fprintf(fp, " ");
        }
    if (!no_sos && !show_sent_num)
        fprintf(fp, "\n\n");
    else
        fprintf(fp, "\n");

}	/* end of write_semsent_horz */


void write_semsent_xml(fp, length, no_sos, show_sent_num)
/* writes out current sentence to xml output file with associated markers.
This is used to write CLAWS vertical version out to XML as well. */
FILE	*fp;
int	length;				/* length of sentence */
int	no_sos;
int	show_sent_num;
{
int	offset = 0,			/* length of current line */
	start_sent_shown = FALSE,
	end_sent_shown = FALSE,
	show_end_now = FALSE,
	x, temp, addlen;
char	tempsupp[100000],
	idstring[100000];

    sentctr += 1; itemctr = 0; idstring[0] = EOS;
    for (x = 0; x <= length; x++)
        {
/*        fprintf(fp, "pos = %d start = %d end = %d show = %d ", x, start_sent_shown, end_sent_shown, show_end_now);
*/
        if (!strcmp(SBREAKTAG, buffer[x].syntag) || !strcmp(SBREAKTAG, buffer[x].word))
            {
            if (show_sent_num)
                {
                addlen = strlen(buffer[x].ref) + 1;
                offset = addlen;
                fprintf(fp, "\n\n%s ", buffer[x].ref);
                }
            continue;
            }
        /* check for supp file references (apart from when no supp file) */
        if (is_supp_ref(buffer[x].word) && (vert_xml || vert_xml_map || semantic_xml || semantic_xml_map))
            get_string_from_supp(buffer[x].word, tempsupp);
        else
            strcpy(tempsupp, buffer[x].word);

        /* check for SGML tags and entities - CLAWS tagged NULL */
        if (tempsupp[0] == '<')
            {
            /* any start/end SGML tag triggers end of sentence to avoid balancing probs */
            addlen = strlen(tempsupp) + 1;
            addlen += 5;
            /* only show end tag once after start tag shown */
            if (start_sent_shown)
                {
                if (!end_sent_shown) show_end_now = TRUE;
                end_sent_shown = TRUE;
                }
                
            if (offset+addlen >= LINELENGTH)
                {
                fprintf(fp,"\n");
                /* eoln reached */
                offset = addlen;
                }
            else
                {
                offset += addlen;
                }

            if (start_sent_shown && show_end_now)
                 fprintf(fp, "</s> %s ", tempsupp);
            else
                 fprintf(fp, "%s ", tempsupp);
            show_end_now = FALSE;
            }
        else
            {
            /* convert " to YQUO */
            if (tempsupp[0] == '"' || buffer[x].syntag[0] == '"')
                {
                strcpy(buffer[x].syntag, "YQUO");
                }
            itemctr += 1;
            /* <s> interferes with xml declaration for first sentence */
            if (!no_sos && sentctr > 1 && itemctr == 1)
                {
                fprintf(fp, "<s> ");
                offset += 4;
                start_sent_shown = TRUE;
                }
            sprintf(idstring, "%d.%d", sentctr, itemctr);
            if (vert_xml || vert_xml_map)
                addlen = strlen(idstring) + 6 + strlen(tempsupp) + 14 + strlen(buffer[x].syntag);
            else
                addlen = strlen(idstring) + 6 + strlen(tempsupp) + 21 + strlen(buffer[x].syntag) + strlen(buffer[x].semtag);
            if (offset+addlen >= LINELENGTH)
                {
                fprintf(fp,"\n");
                /* eoln reached */
                offset = addlen;
                }
            else
                {
                offset += addlen;
                }

            if (vert_xml || vert_xml_map)
                fprintf(fp, "<w id=\"%s\" pos=\"%s\">%s</w>", idstring, buffer[x].syntag, tempsupp);
            else
                fprintf(fp, "<w id=\"%s\" pos=\"%s\" sem=\"%s\">%s</w>", idstring, buffer[x].syntag, buffer[x].semtag, tempsupp);
            fprintf(fp, " ");
            }
        }
    if (!no_sos && !show_sent_num && sentctr > 1 && !end_sent_shown && start_sent_shown)
        fprintf(fp, "</s>\n\n");
    else
        fprintf(fp, "\n");

}	/* end of write_semsent_xml */


void get_string_from_supp(ref, result)
/* return the text from the supp file */
char	*ref, *result;
{
char	hold[WORDSIZE], hold2[100000];
int	chars_transferred, file_offset;
int	i;
char	c;

    extract_sgml_ref(ref, &chars_transferred, &file_offset, hold);
    if (chars_transferred >= 100000)
        rep_err("Supp file string too long.");
    /* must have specified supplementary file name */
    fseek(ifp1supp, file_offset, 0);
    for (i = 0 ; i < chars_transferred ; i++)
        {
        /* copy text from supp file char by char */
        c = getc(ifp1supp);
        if (c == EOF) rep_err("Supp file retrieval error.");
        /* if flag set replace eoln by space */
        if (noeolnsupp && c == '\n')
            result[i] = ' ';
        else
            result[i] = c;
        }
    result[chars_transferred] = ENDOFSTR;
    /* perhaps recursive in some cases */
    if (is_supp_ref(result))
	{
	get_string_from_supp(result, hold2);
	strcpy(result, hold2);
	}

}	/* end of get_string_from_supp */


void extract_sgml_ref(sgmlref, chars_transferred, file_offset, tag)
/* extract CLAWS SGML references to supp file */
char	*sgmlref;
int	*chars_transferred, *file_offset;
char	*tag;
{
char	hold[WORDSIZE];
int	i, j;

    /* first part gives no of chars moved to supp file */
    for (i = 2; sgmlref[i] != ';' ; i++)
        hold[i - 2] = sgmlref[i];
    hold[i - 2] = ENDOFSTR;
    sscanf(hold, "%d", chars_transferred);
    i++;
    j = 0;
    /* second part gives file offset */
    for ( ; sgmlref[i] != ';' ; i++)
        {
        hold[j] = sgmlref[i];
        j++;
        }
    hold[j] = ENDOFSTR;
    sscanf(hold, "%d", file_offset);
    i++;
    j = 0;
    /* third part gives SGML tag */
    for ( ; sgmlref[i] != ENDOFSTR; i++)
        {
        hold[j] = sgmlref[i];
        j++;
        }
    hold[j] = ENDOFSTR;
    strcpy(tag, hold);

}	/* end of extract_sgml_ref */
