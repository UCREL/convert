#include <stdlib.h>
#include <ctype.h>

#ifdef sparc
#include <string.h>
#else
#include <strings.h>
#endif

#define COPYRIGHT	" (c) Lancaster University 1997 "
#define UCREL_COPYRIGHT	" (c) Lancaster University 2016 "

#define SBREAKWORD	"----------------------------------------------------"
#define SBREAKTAG       "-----"	/* punctuation tag - sentence break. */

#define STRING(A)	(char *)strcpy((char *)myalloc(strlen(A)+1),A)
#define STRING2(A, B)	(char *)strcat((char *)strcpy((char *)myalloc(strlen(A)+strlen(B)+1),A), B)
#define NEW_STRUCT(X)   (X *)myalloc(sizeof(X))

#define MYSTRING(A)	(char *)strcpy((char *)myalloc(strlen(A)+1),A)

#define NULLTAG         "NULL"  /* claws null tag - used to be " */
#define NULLTAG1        "\""    /* old null tag - so we can cope with old files */

#define VARBRULE	"Df"	/* indicator of varbrule semantic tag in idiom list */

#define ENDOFSTR	'\0'
#define EOS		'\0'	/* end of string marker */
#define EOLN		'\n'	/* end of line */
#define SPACE		' '
#define TRUE		1
#define FALSE		0	
#define NIL	       -1
#define CHRA		(int) 'a'
#define SEMTAG_LINK_MARKER	'['	/* marker of start of extra link info in a semantic tag */
#define SEMTAG_LINK_SEPRT	'.'	/* separator between extra link info in a semantic tag */
#define UNDERSCORE	'_'	/* char between word and tag in horiz tagged files */

#define	SGML_OPEN_DELIMITER	'<'	/* SGML tag delimiters */
#define	SGML_CLOSE_DELIMITER	'>'
#define	SGML_OPEN_DELIMITER_STR		"<"
#define	SGML_CLOSE_DELIMITER_STR	">"

#define MAX_IDIOM	150000	/* max no. entries in semantic idiom list */

#define MAX_TAGSEM	15	/* max no. of semtags on ambiguous entries */
#define	REFTAGDIST	46	/* no of chars between end of ref-no and syntax-tags */

#define	LGWORDSIZE	300	/* large word size */
#define	DECODESIZE	3	/* size of decision code string */
#define REFSIZE		20     	/* max length of reference nos. in infile. */
#define WORDSIZE	50    	/* max length of string */
#define SYNSIZE		10     	/* max length of syntax tag. */
#define SEMSIZE		40     	/* max length of content tag */	

#define	SCREENHEIGHT	24	/* no of lines per screen */
#define SCREENWIDTH	80	/* screen width */
#define LINEPWIDTH	130	/* line printer width */

#define SENTLEN		200	/* max no. of units in sentence */

				/* file stuff */
#define FILENMLEN	500	/* max length of file name */

#define EXTRACT_ALL_TAGS  0	/* use in standard file read next item fns */
#define EXTRACT_FIRST_TAG 1
#define EXTRACT_CONFLATED_TAGS 2

#define TAGSUMMARY	"tagsummary"		/* semantic tag summary file */
#define CLAWSTAGS	"clawstags.c7"		/* CLAWS c7 tag summary file */

#define ENVN_RESOURCE	"PRESOURCES"		/* central resource env var */
#define RMPATH		"/bin/rm"		/* path name for UNIX rm */

/* new types */

typedef struct claws_line_tags_link	/* linked list of CLAWS tags */
	{
	char	*tag,		/* CLAWS tag */
		*pct;		/* percentage */
	struct claws_line_tags_link *next;	/* pointer to next tag */
	} claws_line_tags_ent;

typedef struct SemTagListLink		/* linked list of semantic tags */
	{
	char	*sem;			/* semantic tag */
	struct SemTagListLink *next;	/* pointer to next tag */
	} SemTagListEnt;

typedef struct linklink
	{
	char	linktype;		/* type of link */
	int	linkcn,			/* link counter */
		linkno,			/* number of parts to link */
		linkpo;			/* position in link */
	struct linklink *next;
	} linkent;


/* prototypes */

/* claws_out.c */
void init_claws_line();
int read_claws_line();
int read_claws_line2();
void free_claws_line_tags();

/* semtag_text.c */
linkent *semtag_strip_marker();

/* genutil.c */
void chop_semtag_level_two();
void copyright_message();
void expconv();
void fatal_error();
void fsafeopen();
int  get_no_lines_infile();
int  get_no_words_infile();
void get_username();
void non_fatal_error();
int  open_non_fatal_resource();
int  open_resource_file();
void parsfname();
char plural();
void regular_expression_free();
int  regular_expression_cmp();
int  regular_expression_compile();
int  regular_expression_compare();
void rep_err();
void reverse_string();
char *STRAPP();
char *timestamp();
void warn_err();
void strdog();
char *MYSTRAPP();
void *myalloc();
void myfree();
void SuppFileGetString();
void ExtractSgmlRef();
int IsSuppRef();

