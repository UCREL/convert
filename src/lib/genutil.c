#include <stdio.h>
#include <ctype.h>
#include "util.h"
#include <stdlib.h>

static int totalallocated = 0;

#include <time.h>

#define	PATHDELIM	'/'

void parsfname(old, fname, ext)
/* Take the file name in "old" and remove, if it exists, an extention from
the end of it. Then remove the full path, if it exists. Then copy the
remainder to the "fname" and concatanate the extention "ext" to the
end of string in "fname" */
char *old, *fname, *ext;
{
int i, ti, indx;

    i = strlen(old) - 1;
    while (i >= 0 && (old[i] == ' ' || old[i] == '\t'))
        i--;
    old[i+1] = '\0';
    i = strlen(old);
    while (i > 0 && old[i] != '.' && old[i] != PATHDELIM)
        i--;

    if (old[i] == PATHDELIM)
        {
        ti = i+1;
        i = strlen(old);
        }
    else if (old[i] == '.' && old[i-1] != PATHDELIM)
        {
        ti = i;
        while (ti > 0 && old[ti] != PATHDELIM)
            ti--;
        if (old[ti] == PATHDELIM) ti++;
        }
    else if (old[i] == '.' && old[i-1] == PATHDELIM)
        {
        ti = i;
        i = strlen(old);
        }
    else
        {
        ti = 0;
        if (old[i] != '.')
            i = strlen(old);
        }

    indx = 0;
    while (ti < i)
        fname[indx++] = old[ti++];
    fname[indx] = '\0';
    strcat(fname,ext);

}	/* end of parsfname */


char *STRAPP(s1, s2)
/* append s2 on to s1 where s1 is a string created by the STRING macro */
char	*s1, *s2;
{
char	*p;

    /* malloc the space needed then copy strings across */
    p = (char *)strcat((char *)strcpy((char *)myalloc(strlen(s1) + strlen(s2) + 1), s1), s2);
    /* free old copy of s1 */
    myfree(s1);
    return(p);

}	/* end of STRAPP */


char *MYSTRAPP(s1, s2)
/* append s2 on to s1 where s1 is a string created by the MYSTRING macro */
char	*s1, *s2;
{
char	*p;

    /* malloc the space needed then copy strings across */
    p = (char *)strcat((char *)strcpy((char *)myalloc(strlen(s1) + strlen(s2) + 1), s1), s2);
    /* free old copy of s1 */
    myfree(s1);
    return(p);

}	/* end of MYSTRAPP */


void *myalloc(size)
int	size;
{
void *m;
long *poo;
long ret;
int	debugflag;

    debugflag = FALSE;
    if (debugflag)
    {
    if (size == 0)
        {
        printf("allocating size ZERO!!!!!!!!!!!%c\n", 0x07);
        return(0);
        }    
    m = (void *)malloc(size+(sizeof(long)*2));
    if (m == NULL)
        {
        printf("myalloc %d failed!!!\n", size);
        return(NULL);
        }
    poo = (long *)m;
    poo[0] = (long)0xAC3D;
    poo[1] = (long)size;
    totalallocated += size;


    ret = ((long)m)+(sizeof(long)*2);

/*    if (debugflag)
        printf("allocated %d bytes (loc = 0x%X,  total allocated %d )\n", size, ret, totalallocated);
*/
    return((void *)ret);
    }

else
    {
    if (size == 0)
        printf("Warning - malloc called with size 0\n");
    
    m = malloc(size);
    if (m == NULL)
        {
        printf("myalloc %d failed!!!\n", size);
        return(NULL);
        }
    
    return(m);
    }

}	/* end of myalloc */


void myfree(addr)
long addr;
{
int debugflag = FALSE;

    if (!addr)
        {
#ifdef debug
        printf("ERROR!  trying free non-allocated or corrupted space (addr 0x%X)\n", addr);
#endif
        return;
        }

    if (debugflag)
        {
        long *poo = (long *)((long)addr - (sizeof(long)*2));
        long id = poo[0];
        long size = poo[1];

        if (id == 0xDEAD)
            {
#ifdef debug
            printf("ERROR!  trying to free already freed space size=%d (addr %X)\n",
                size, addr);
#endif
            return;
            }
        else
        if (id == 0xAC3D)
            {
            totalallocated -= size;
            poo[0] = 0xDEAD;

/*            if (debugflag)
                printf("freeing %d (addr 0x%X, total allocated %d)\n", size, addr , totalallocated);
*/
            free(poo);
            }
        else
            {
#ifdef debug
            printf("ERROR!  trying free non-allocated or corrupted space %d (addr 0x%X)\n",
                size, poo);
#endif
            }
        }
    else
        free((char *)addr);

}	/* end of myfree */


void copyright_message(name, version)
/* output a formatted copyright message */
char	*name,			/* program name */
	*version;		/* version number */
{
    fprintf(stdout, "%s:%sVERSION %s\n", name, UCREL_COPYRIGHT, version);

}	/* end of copyright_message */


char *timestamp()
/* return a string containing date and time */
{
time_t  tm;

    time(&tm);
    return(ctime(&tm));

}	/* end of timestamp */


void rep_err(s1)
/* report error then stop */
char	*s1;
{
    fprintf(stderr, "ERROR : %s\n", s1);
    exit(1);

}	/* end of rep_err */


void fatal_error(s1, s2)
/* report error then stop */
char	*s1, *s2;
{
    fprintf(stderr, "ERROR : %s : %s\n", s1, s2);
    exit(1);

}	/* end of fatal_error */


void non_fatal_error(s1, s2)
/* report error then continue */
char	*s1, *s2;
{
    fprintf(stderr, "WARNING : %s : %s\n", s1, s2);

}	/* end of non_fatal_error */


void warn_err(s1)
/* report error then continue */
char	*s1;
{
    fprintf(stderr, "WARNING : %s\n", s1);

}	/* end of warn_err */


int open_resource_file(fp, username, filename)
/* open a specified file in current directory or, if that fails, from a
standard backup directory which is defined by an environmental variable */
FILE	**fp;		/* file pointer return to calling proc */
char	*username,	/* who is using program? TODO: remove this */
	*filename;	/* file to be opened */
{
int	using_backup = FALSE;
char	*ucrel_resources;	/* env var giving resource directory */
char	tempfn[100];

    if ((*fp = fopen(filename, "r")) == NULL)
        {
#ifdef debug
        non_fatal_error(filename, "not found in current directory (trying environmental variable)");
#endif
        /* as a last resort look in a directory specified by an environmental variable */
        ucrel_resources = getenv(ENVN_RESOURCE);
        if (ucrel_resources != NULL)
            {
            sprintf(tempfn, "%s/%s", ucrel_resources, filename);
            if ((*fp = fopen(tempfn, "r")) == NULL)
                {
                sprintf(tempfn, "File not found in %s directory.", ucrel_resources);
                fatal_error(tempfn, filename);
                }
            }
        else
            {
            sprintf(tempfn, "File not found and $%s not defined.", ENVN_RESOURCE);
            fatal_error(tempfn, filename);
            }
        }

    return(using_backup);

}	/* end of open_resource_file */


int open_non_fatal_resource(fp, basename, filename, message)
/* return a file pointer to a file found either in the current directory,
or in resource directory specified in an enviromental variable */
/* return TRUE if succesful */
FILE	**fp;		/* file pointer return to calling proc */
char	*basename,	/* basename of file to open */
	*filename,	/* full name of file opened (if any) */
	*message;	/* return (error) message if any */
{
char	*ucrel_resources;	/* env var giving resource directory */

    strcpy(filename, basename);

    if ((*fp = fopen(basename, "r")) == NULL)
        {
#ifdef debug
        non_fatal_error(basename, "not found in current directory ... trying environmental variable");
#endif
        /* as a last resort look in a directory specified by an environmental variable */
        ucrel_resources = getenv(ENVN_RESOURCE);
        if (ucrel_resources != NULL)
            {
            sprintf(filename, "%s/%s", ucrel_resources, basename);
            if ((*fp = fopen(filename, "r")) == NULL)
                {
                sprintf(message, "No help available. \nFile %s not found in %s directory.", basename, ucrel_resources);
                return(FALSE);
                }
            }
        else
            {
            sprintf(message, "No help available. \nFile %s not found and $%s not defined.", basename, ENVN_RESOURCE);
            return(FALSE);
            }
        }

    return(TRUE);

}	/* end of open_non_fatal_resource */


void fsafeopen(fp, fn)
/* open an output file and return the pointer, try tmp if unsucessful */
FILE	**fp;
char	*fn;
{
char	tempfn[FILENMLEN];

    *fp = fopen(fn, "w");
    if (*fp == NULL)
        {
        fprintf(stderr, "ERROR: Can't open file: '%s' ", fn);
        fprintf(stderr, "Writing to /tmp/%s instead \n", fn);
        sprintf(tempfn, "/tmp/%s", fn);
        *fp = fopen(tempfn, "w");
        if (*fp == NULL)
            {
            fputs("Write to /tmp failed so I'm giving up!!", stderr);
            exit(1);
            }
        }

}	/* end of fsafeopen */


void expconv(s1, ln)
/* convert string to regular expression format */
char	*s1;
int	ln;
{
char	temp[255];
int	i, j = 0;

    temp[j++] = '^';
    for (i = 0; i < ln; i++)
        {
        if (s1[i] == '*')
            {
            temp[j++] = '.';
            temp[j++] = '*';
            }
        else if (s1[i] == '.')
            {
            temp[j++] = '\\';
            temp[j++] = '.';
            }
        else
            {
            temp[j++] = s1[i];
            }
        }
    temp[j++] = '$';
    temp[j] = ENDOFSTR;
    strcpy(s1, temp);

}	/* end of expconv */


int regular_expression_cmp(sreg, scur)
/* match regular expression with current string */
char	*sreg, *scur;
{
char	*exp;
int	value;

    regular_expression_compile(sreg, exp);
    value = regular_expression_compare(exp, scur);
    regular_expression_free(exp);

    return(value);

/* TODO copy #ifdefs for SYSV and BSD into here from page 425 of Vol6a Motif book */
/* use compile and compare functs below */
/*    if (re_comp(sreg)) rep_err("regular expression fault");

    value = re_exec(scur);
    if (value == -1)
        rep_err("Invalid regular expression.");
    else
        return(value);
*/
}	/* end of regular_expression_cmp */


void get_username(username)
/* get the username of the person running the program */
char	*username;
{
    strcpy(username, getenv("USER"));

}	/* end of get_username */


void reverse_string(word, length)
/* reverse a string */
char	*word;
int	length;
{
char	copy[WORDSIZE];
int	x;

    length--;
    for (x = length; x >= 0; x--)
        {
        copy[length - x] = word[x];
        }
    copy[length + 1] = EOS;

    strcpy(word, copy);

}	/* end of reverse_string */


int get_no_lines_infile(fname)
/* return the number of lines in a file using UNIX 'wc' */
char	*fname;
{
FILE	*pfp;
char	command[100], temp[100];

    sprintf(command, "wc -l %s", fname);
    pfp = popen(command, "r");
    /* pipe failed ? */
    if (!pfp) return(0);

    /* extract number of lines from wc output */
    fscanf(pfp, "%s", temp);
    pclose(pfp);

    return(atoi(temp));

}	/* end of get_no_lines_infile */


int get_no_words_infile(fname)
/* return the number of words in a file using UNIX 'wc' */
char	*fname;
{
FILE	*pfp;
char	command[100], temp[100];

    sprintf(command, "wc -w %s", fname);
    pfp = popen(command, "r");
    /* pipe failed ? */
    if (!pfp) return(0);

    /* extract number of words from wc output */
    fscanf(pfp, "%s", temp);
    pclose(pfp);

    return(atoi(temp));

}	/* end of get_no_words_infile */


char	plural(number)
/* if the number is anything other than 1 return 's' */
int	number;
{
    if (number == 1)
        return(' ');
    else
        return('s');

}	/* end of plural */


int regular_expression_compile(sreg, exp)
/* compile regular expression into pattern matching library */
/* return TRUE only if successful */
char	*sreg,		/* regular expression */
	*exp;		/* compiled expression or error string */
/* TODO do we need to pass **exp to get the return for SYSV? */
{
#ifndef SYSV
extern char *re_comp();
#endif

#ifdef SYSV
    if (!(exp = regcmp(sreg, NULL)))
        {
        printf("Error with regcmp(%s)\n", sreg);
#else /* BSD */
    if (exp = re_comp(sreg))
        {
        printf("Error with re_comp(%s): %s\n", sreg, exp);
#endif
        return(FALSE);
        }
    else
        return(TRUE);

}	/* end of regular_expression_compile */


int regular_expression_compare(exp, stext)
/* compare a previously compiled regular expression with a string 
and return the result */
char	*exp,		/* compiled regex or error from above */
	*stext;		/* string to compare with */
{
#ifdef SYSV
        /* returns NULL if matched failed */
        return(regex(exp, stext, NULL) != NULL);
#else /* BSD */
        /* -1 on error, 0 if no match, 1 if match */
        return(re_exec(stext) > 0);
#endif

}	/* end of regular_expression_compare */


void regular_expression_free(exp)
/* SYSV requires you to free the compiled expression */
char	*exp;
{
#ifdef SYSV
    free(exp);
#endif
}	/* end of regular_expression_free */


void strdog(s1, sprep)
/* prepend the second string into the first (opposite of strcat) */
/* doesn't check that s1 is long enough !! */
char	*s1, *sprep;
{
char	*temp;

    /* copy the first string temporarily out the way */
    temp = STRING(s1);
    /* copy over the new bit */
    strcpy(s1, sprep);
    /* append the original */
    strcat(s1, temp);
    /* free the temp string */
    free(temp);
    /* cor blimey that was easy, why isn't it a standard function? */

}	/* end of strdog */


void chop_semtag_level_two(semtag)
/* chop off characters after the first upper case letter and the first 
numbered subdivision */
char	*semtag;
{
int	i;

    i = 1;
    /* need to chop gender markers, idiom markers, + and - and slash tags
    and rarity markers */
    while (semtag[i] != EOS && !strchr("+-mfnc[./@%", semtag[i]))
        i++;
    semtag[i] = EOS;

}	/* end of chop_semtag_level_two */
