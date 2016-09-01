#include "convert.h"

FILE   *ifp, *ofp, *ifp1supp;

int	sentctr		= 0,		/* sentence counter */
	itemctr		= 0,		/* item counter */
	infile_empty	= FALSE,	/* flags ... */
	horz_vert	= FALSE,	/* true if -h2v flag used */
	horz_xkwic	= FALSE,	/* -h2k flag */
	vert_horz	= FALSE,	/* true if -v2h flag used */
	vert_horz_supp	= FALSE,	/* true if -v2hsupp flag used */
        vert_horz_supp_map  = FALSE,        /* true if -v2hsuppmap flag used */
	vert_vert_supp	= FALSE,	/* true if -v2vsupp flag used */
	vert_vert_map	= FALSE,	/* true if -v2vmap flag used */
	vert_xkwic	= FALSE,	/* true if -v2k flag used */
	vert_xkwic_supp	= FALSE,	/* true if -v2ksupp flag used */
	vert_xml	= FALSE,        /* true if -v2x flag used */
	vert_xml_map	= FALSE,        /* true if -v2xmap flag used */
	parse_vert	= FALSE,	/* true if -p2v flag used */
	parse_horz	= FALSE,	/* true if -p2h flag used */
	semantic_horz   = FALSE,        /* true if -s2h flag used */
	semantic_horz_supp   = FALSE,   /* true if -s2hsupp flag used */
	semantic_horz_strip   = FALSE,  /* true if -s2hstrip flag used */
	semantic_horz_strip_pos   = FALSE,  /* true if -s2hstripos flag used */
	semantic_xkwic   = FALSE,        /* true if -s2k flag used */
	semantic_xkwic_supp   = FALSE,   /* true if -s2ksupp flag used */
	semantic_xml    = FALSE,        /* true if -s2x flag used */
	semantic_xml_map = FALSE,        /* true if -s2xmap flag used */
	semantic_xml_nosupp    = FALSE,        /* true if -s2xnosupp flag used */
	lemma_treetag	= FALSE,	/* true if -l2t flag used */
	bnc_vert	= FALSE,	/* true if -b2v flag used */
	bnc_horz	= FALSE,	/* true if -b2h flag used */
	vard		= FALSE,	/* true if -vard flag used */
	nwulem		= FALSE,	/* true if -nwulem flag used */
	cup_text	= FALSE,	/* true if -cup flag used */
	parallel	= FALSE,	/* true if -para flag used */
	remove_rarity	= FALSE,	/* true if -rare flag used */
	fletter	= FALSE,		/* true if -fletter flag used */
	noeolnsupp = FALSE,		/* true if -noeolnsupp flag used, means no end of line char copied from supp ref */
	numaps,				/* number of entries in mapping */
	length;				/* length of sentence */

char	*sentbreak;			/* sentence break */

puncent	punc[MAX_PUNC];			/* complete punctuation array */

buffent	buffer[BUFFERSIZE];		/* complete buffer */

mapent mapping[100000];			/* complete mapping */

main(argc, argv)
int 	argc;
char	*argv[];
{
int	opcount;			/* current argument number */
char	command[40];
int	no_sos = FALSE,			/* include start of sentence marks in output */
	show_sent_num = FALSE;		/* show sent ref no in horiz output */

    /* check command-line options */
    if (argc <= 2)
        help();
    opcount = 1;

/* all these flags should be one enumerated type */

    if (strcmp(argv[opcount], "-h2v") == 0)
        horz_vert = TRUE;
    else if (strcmp(argv[opcount], "-h2k") == 0)
        horz_xkwic = TRUE;
    else if (strcmp(argv[opcount], "-s2k") == 0)
        semantic_xkwic = TRUE;
    else if (strcmp(argv[opcount], "-s2ksupp") == 0)
        semantic_xkwic_supp = TRUE;
    else if (strcmp(argv[opcount], "-s2x") == 0)
        semantic_xml = TRUE;
    else if (strcmp(argv[opcount], "-s2xmap") == 0)
        semantic_xml_map = TRUE;
    else if (strcmp(argv[opcount], "-s2xnosupp") == 0)
        semantic_xml_nosupp = TRUE;
    else if (strcmp(argv[opcount], "-v2x") == 0)
        vert_xml = TRUE;
    else if (strcmp(argv[opcount], "-v2xmap") == 0)
        vert_xml_map = TRUE;
    else if (strcmp(argv[opcount], "-v2h") == 0)
        vert_horz = TRUE;
    else if (strcmp(argv[opcount], "-v2hsupp") == 0)
        vert_horz_supp = TRUE;
    else if (strcmp(argv[opcount], "-v2hsuppmap") == 0)
        vert_horz_supp_map = TRUE;
    else if (strcmp(argv[opcount], "-v2vsupp") == 0)
        vert_vert_supp = TRUE;
    else if (strcmp(argv[opcount], "-v2vmap") == 0)
        vert_vert_map = TRUE;
    else if (strcmp(argv[opcount], "-cup") == 0)
        {
        cup_text = TRUE;
        vert_vert_supp = TRUE;
        no_sos = TRUE;
        }
    else if (strcmp(argv[opcount], "-nwulem") == 0)
        nwulem = TRUE;
    else if (strcmp(argv[opcount], "-p2h") == 0)
        parse_horz = TRUE;
    else if (strcmp(argv[opcount], "-p2v") == 0)
        parse_vert = TRUE;
    else if (strcmp(argv[opcount], "-s2h") == 0)
        semantic_horz = TRUE;
    else if (strcmp(argv[opcount], "-s2hsupp") == 0)
        semantic_horz_supp = TRUE;
    else if (strcmp(argv[opcount], "-b2v") == 0)
        bnc_vert = TRUE;
    else if (strcmp(argv[opcount], "-b2h") == 0)
        bnc_horz = TRUE;
    else if (strcmp(argv[opcount], "-s2hstrip") == 0)
        semantic_horz_strip = TRUE;
    else if (strcmp(argv[opcount], "-s2hstripos") == 0)
        semantic_horz_strip_pos = TRUE;
    else if (strcmp(argv[opcount], "-l2t") == 0)
        lemma_treetag = TRUE;
    else if (strcmp(argv[opcount], "-vard") == 0)
        vard = TRUE;
    else if (strcmp(argv[opcount], "-v2k") == 0)
        vert_xkwic = TRUE;
    else if (strcmp(argv[opcount], "-v2ksupp") == 0)
        vert_xkwic_supp = TRUE;
    else if (strcmp(argv[opcount], "-help") == 0)
        help();
    else
        fprintf(stderr, "Invalid option : %s\n", argv[opcount]);
    opcount++;
    if (!strcmp(argv[opcount], "-nosos"))
        {
        no_sos = TRUE;
        opcount++;
        }
    if (!strcmp(argv[opcount], "-sentnum"))
        {
        show_sent_num = TRUE;
        opcount++;
        }
    if (!strcmp(argv[opcount], "-para"))
        {
        parallel = TRUE;
        opcount++;
        }
    if (!strcmp(argv[opcount], "-rare"))
        {
        remove_rarity = TRUE;
        opcount++;
        }
    if (!strcmp(argv[opcount], "-fletter"))
        {
        fletter = TRUE;
        opcount++;
        }
    if (!strcmp(argv[opcount], "-noeolnsupp"))
        {
        noeolnsupp = TRUE;
        opcount++;
        }

    if (opcount >= argc - 1)
        help();

    if (semantic_xkwic_supp || vert_horz_supp || vert_vert_supp || nwulem || semantic_xml || vert_xml || semantic_horz_supp || vert_xkwic_supp)
        {
        if (argc < 5) rep_err("Please specify a supp file.");
        opcount = argc - 3;
        }
    else if (lemma_treetag || vard || vert_vert_map)
        {
        if (argc < 5) rep_err("Please specify a mapping file.");
        opcount = argc - 3;
        }
    else if (semantic_xml_map || vert_xml_map || vert_horz_supp_map)
        {
        if (argc < 6) rep_err("Please specify a supp file and a mapping file.");
        opcount = argc - 4;
        }
    else
        opcount = argc - 2;

    /* try to open infile */
    if ((ifp = fopen(argv[opcount], "r")) == NULL)
        {
        fprintf(stderr, "\n\n*** file %s not found ***\n\n", argv[opcount]);
        exit(1);
        }

    fputs("CONVERT:", stdout);
    fputs(UCREL_COPYRIGHT, stdout);
    fputs("\nVERSION ", stdout);
    fputs(VERSION, stdout);
    fputs(" : ", stdout);

    sentbreak = STRING(SBREAKWORD);
    fprintf(stdout, "Infile is %s : ", argv[opcount]);
    opcount++;
    fprintf(stdout, "Outfile is %s\n", argv[opcount]);
    ofp = fopen(argv[opcount], "w");
    initialise_punc();
    if (horz_vert)
        {
        fprintf(stdout, "Horizontal to vertical conversion.\n");
/*        length = read_sent_horz(ifp);
        while (!infile_empty)
            {
            write_sent_vert(ofp, length, no_sos);
            length = read_sent_horz(ifp);
            }
*/
        while (!infile_empty)
            {
            length = read_sent_horz(ifp);
            write_sent_vert(ofp, length, no_sos);
            }
        }
    else if (horz_xkwic)
        {
        fprintf(stdout, "Horizontal to xkwic conversion.\n");
        length = read_sent_horz(ifp);
        while (!infile_empty)
            {
            write_sent_xkwic(ofp, length, no_sos);
            length = read_sent_horz(ifp);
            }
        }
    else if (semantic_xkwic)
        {
        fprintf(stdout, "Semantic to xkwic conversion.\n");
        while (!infile_empty)
            {
	    read_semantic(ifp, FALSE, FALSE);
            write_semsent_xkwic(ofp, length, no_sos);
            }
        }
    else if (semantic_xkwic_supp)
        {
        fprintf(stdout, "Semantic to xkwic conversion with supp file.\n");
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");
        while (!infile_empty)
            {
	    read_semantic(ifp, FALSE, FALSE);
            write_semsent_xkwic(ofp, length, no_sos);
            }
        }
    else if (lemma_treetag)
        {
        fprintf(stdout, "Lemmatised to TreeTagger conversion.\n");
	numaps = read_map(argv[++opcount]);
        while (!infile_empty)
            {
	    read_semantic(ifp, FALSE, FALSE);
	    map_tags();
            write_semsent_xkwic(ofp, length, no_sos);
            }
        }
    else if (vard)
        {
	char	nword[1000];
        fprintf(stdout, "VARiant Dectector conversion.\n");
	numaps = read_map3(argv[++opcount]);
        while (!infile_empty)
            {
	    infile_empty = get_sgmlraw_text_word(ifp, nword);
	    map_word(nword);
            fprintf(ofp, "%s \n", nword);
            }
        }
    else if (semantic_xml)
        {
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");
        fprintf(stdout, "Semantic to XML conversion.\n");
        while (!infile_empty)
            {
	    read_semantic(ifp, FALSE, FALSE);
            write_semsent_xml(ofp, length, no_sos, show_sent_num);
            }
        fclose(ifp1supp);
        }
    else if (semantic_xml_map)
        {
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");
        fprintf(stdout, "Semantic to XML conversion with POS tag mapping.\n");
	numaps = read_map(argv[++opcount]);
        while (!infile_empty)
            {
	    read_semantic(ifp, FALSE, FALSE);
	    map_tags();
            write_semsent_xml(ofp, length, no_sos, show_sent_num);
            }
        fclose(ifp1supp);
        }
    else if (semantic_xml_nosupp)
        {
        fprintf(stdout, "Semantic to XML conversion with no supp file.\n");
        while (!infile_empty)
            {
	    read_semantic(ifp, FALSE, FALSE);
            write_semsent_xml(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (vert_xml)
        {
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");
        fprintf(stdout, "Vertical to XML conversion.\n");
        while (!infile_empty)
            {
	    read_sent_vert(ifp);
            write_semsent_xml(ofp, length, no_sos, show_sent_num);
            }
        fclose(ifp1supp);
        }
    else if (vert_xml_map)
        {
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");
        fprintf(stdout, "Vertical to XML conversion with POS tag mapping and supp file.\n");
		numaps = read_map(argv[++opcount]);
        while (!infile_empty)
            {
	    	read_sent_vert(ifp);
	    	map_tags();
            write_semsent_xml(ofp, length, no_sos, show_sent_num);
            }
        fclose(ifp1supp);
        }
    else if (vert_horz)
        {
        fprintf(stdout, "Vertical to horizontal conversion.\n");
        while (!infile_empty)
            {
            read_sent_vert(ifp);
            write_sent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (vert_horz_supp)
        {
        fprintf(stdout, "Vertical to horizontal conversion with supp file.\n");
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");

        while (!infile_empty)
            {
            read_sent_vert(ifp);
            write_sent_horz(ofp, length, no_sos, show_sent_num);
            }
        fclose(ifp1supp);
        }
    else if (vert_horz_supp_map)
        {
        fprintf(stdout, "Vertical to horizontal conversion with supp file and POS tag mapping.\n");
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");
        numaps = read_map(argv[++opcount]);

        while (!infile_empty)
            {
            read_sent_vert(ifp);
            map_tags();
            write_sent_horz(ofp, length, no_sos, show_sent_num);
            }
        fclose(ifp1supp);
        }
    else if (vert_xkwic)
        {
        fprintf(stdout, "Vertical to IMS-CWB (xkwic) conversion.\n");
        while (!infile_empty)
            {
            read_sent_vert(ifp);
            write_sent_xkwic(ofp, length, no_sos);
            }
        }
    else if (vert_xkwic_supp)
        {
        fprintf(stdout, "Vertical to IMS-CWB (xkwic) conversion with supp file.\n");
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");

        while (!infile_empty)
            {
            read_sent_vert(ifp);
            write_sent_xkwic(ofp, length, no_sos);
            }
        fclose(ifp1supp);
        }
    else if (vert_vert_supp)
        {
        fprintf(stdout, "Vertical to vertical conversion with supp file.\n");
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");

        while (!infile_empty)
            {
            read_sent_vert(ifp);
/*            write_sent_horz(ofp, length, no_sos, show_sent_num);*/
            write_sent_vert(ofp, length, no_sos);
            }
        fclose(ifp1supp);
        }
    else if (vert_vert_map)
        {
        fprintf(stdout, "Vertical to vertical conversion with mapping.\n");
        numaps = read_map(argv[++opcount]);

        while (!infile_empty)
            {
            read_sent_vert(ifp);
	    map_tags();
            write_sent_vert(ofp, length, no_sos);
            }
        fclose(ifp1supp);
        }
    else if (nwulem)
        {
        fprintf(stdout, "NWU format conversion with supp file.\n");
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");

        while (!infile_empty)
            {
            read_sent_lemma(ifp);
            write_sent_lemma(ofp, length, no_sos);
            }
        fclose(ifp1supp);
        }
    else if (parse_horz)
        {
        fprintf(stdout, "Parsed to horizontal conversion.\n");
        while (!infile_empty)
            {
            read_parse(ifp);
            write_sent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (parse_vert)
        {
        fprintf(stdout, "Parsed to vertical conversion.\n");
        while (!infile_empty)
            {
            read_parse(ifp);
            write_sent_vert(ofp, length, no_sos);
            }
        }
    else if (semantic_horz)
        {
        puts("Semantic to horizontal conversion.");
        while (!infile_empty)
            {
            read_semantic(ifp, FALSE, FALSE);
            write_semsent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (semantic_horz_supp)
        {
        if ((ifp1supp = fopen(argv[++opcount], "r")) == NULL)
            rep_err("Can't find supp file.");

        puts("Semantic to horizontal conversion with supp file.");
        while (!infile_empty)
            {
            read_semantic(ifp, FALSE, FALSE);
            write_semsent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (semantic_horz_strip)
        {
        puts("Semantic to horizontal conversion with tag strip.");
        while (!infile_empty)
            {
            read_semantic(ifp, TRUE, FALSE);
            write_semsent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (semantic_horz_strip_pos)
        {
        puts("Semantic to horizontal conversion with tag strip and POS reduction.");
        while (!infile_empty)
            {
            read_semantic(ifp, TRUE, TRUE);
            write_semsent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else if (bnc_vert)
        {
        puts("BNC to vertical conversion.");
        while (!infile_empty)
            {
            read_bnc_text(ifp, TRUE, FALSE);
            write_sent_vert(ofp, length, no_sos);
            }
        }
    else if (bnc_horz)
        {
        puts("BNC to horizontal conversion.");
        while (!infile_empty)
            {
            read_bnc_text(ifp);
            write_sent_horz(ofp, length, no_sos, show_sent_num);
            }
        }
    else
        {
        printf("No changes!!\n");
        sprintf(command, "cp %s %s", argv[opcount-1], argv[opcount]);
        system(command);
        }
    fclose(ifp);
    fclose(ofp);
    exit(0);

}	/* end of main */
