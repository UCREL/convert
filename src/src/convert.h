#include <stdio.h>
#include <ctype.h>
#include "util.h"

#define	VERSION		"7"
#define	BUFFERSIZE	5000	/* complete buffer size. */
#define MAX_PUNC 	13     	/* max no. entries in punc array. */
#define PUNCSIZE	5      	/* max length of item in punc array. */
#define MARKSIZE	10	/* max length of marker string */
#define LINELENGTH	80	/* screen width */
#define ALTNO		10	/* max number of alternative tags/words */
#define CHRO		(int) '0'
#define BASE		10
#define SBREAKTAG       "-----"	/* punctuation tag - sentence break. */
#define	MAXSTRING	2000

typedef struct
	{	 			/* entry in punctuation array */
	char	*item;
	} puncent;

typedef struct
	{				/* holds buffer entry */
	int	wdln,			/* word length */
		sxln;
	char	ref[REFSIZE],		/* ref number */
		word[1000],		/* one word */
		syntag[1000],	/* syntax tag */
		semtag[1000],	/* semantic tag */
		lemma[1000];	/* lemma */
	} buffent;

typedef struct
	{				/* one mapping entry */
	char	from[1000],
		to[1000],
		type[1000];
	} mapent;

extern	char	*sentbreak;		/* sentence break */

extern	puncent	punc[MAX_PUNC];		/* complete punctuation array */

extern	buffent	buffer[BUFFERSIZE];	/* complete buffer */

extern	mapent mapping[100000];		/* complete mapping */

extern	int	sentctr,		/* sentence counter */
	itemctr,			/* item counter */
	infile_empty,			/* flags ... */
	horz_vert,			/* true if -h2v flag used */
	horz_xkwic,			/* -h2k flag */
	vert_horz,			/* true if -v2h flag used */
	vert_horz_supp,			/* true if -v2hsupp flag used */
	vert_horz_supp_map,		/* true if -v2hsuppmap flag used */
	vert_vert_supp,			/* true if -v2vsupp flag used */
	vert_vert_map,			/* true if -v2vmap flag used */
	vert_xkwic,			/* true if -v2k flag used */
	vert_xkwic_supp,		/* true if -v2ksupp flag used */
	vert_xml,		        /* true if -v2x flag used */
	vert_xml_map,		        /* true if -v2xmap flag used */
	parse_vert,			/* true if -p2v flag used */
	parse_horz,			/* true if -p2h flag used */
	semantic_horz,                  /* true if -s2h flag used */
	semantic_horz_supp,             /* true if -s2hsupp flag used */
	semantic_horz_strip,            /* true if -s2hstrip flag used */
	semantic_xkwic,        /* true if -s2k flag used */
	semantic_xkwic_supp,   /* true if -s2ksupp flag used */
	semantic_xml,		        /* true if -s2x flag used */
	semantic_xml_map,               /* true if -s2xmap flag used */
	semantic_xml_nosupp,            /* true if -s2xnosupp flag used */
	lemma_treetag,			/* true if -l2t flag used */
	bnc_vert,			/* true if -b2v flag used */
	bnc_horz,			/* true if -b2h flag used */
	vard,				/* true if -vard flag used */
	nwulem,				/* true if -nwulem flag used */
	cup_text,			/* true if -cup flag used */
	parallel,			/* true if -para flag used */
        remove_rarity,		        /* true if -rare flag used */
	fletter,			/* true if -fletter flag used */
	noeolnsupp,			/* true if -noeolnsupp flag used, means no end of line char copied from supp ref */
	numaps,				/* number of entries in mapping */
	length;				/* length of sentence */

extern	FILE   *ifp, *ofp, *ifp1supp;

/* prototypes */

/* horiz.c */
int read_sent_horz();
void write_sent_horz();
void get_string_from_supp();
void extract_sgml_ref();

/* util.c */
void help();
int is_punc();
int member();
int tenpow();
void itostr();
void separate_tag();

/* vertical.c */
int is_supp_ref();

/* bnc_text.c */
void read_bnc_text();

/* map.c */
int read_map();
void map_tags();
void map_word();

/* sgml_text.c */
int get_sgmlraw_text_word();
