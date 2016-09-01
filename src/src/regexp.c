 /*
  *
  * MOT Database  Wildcard search - function. 
  *
  * (c) Kielikone Oy,  J-P Juntunen 2003
  * Donated to Lancaster CLAWS Project.
  *
  */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "regexp.h"


/*
The regular_expression defines the pattern to search for.  Upper- and
lower-case are always ignored.  Blank lines never match.  The expression
should be quoted to prevent file-name translation.
x      An ordinary character (not mentioned below) matches that character.
'\\'    The backslash quotes any character.  \"\\$\" matches a dollar-sign.
       beginning of a line.
'$'    A dollar-sign at the end of an expression matches the end of a line.
'.'    A period matches any character except \"new-line\".
'*'    An expression followed by an asterisk matches zero or more
       occurrances of that expression: \"fo* \" matches \"f\", \"fo\"
       \"foo\", etc.
'+'    An expression followed by a plus sign matches one or more
       occurrances of that expression: \"fo+\" matches \"fo\", etc.
'[]'   A string enclosed in square brackets matches any character in
       that string, but no others.  If the first character in the
       string is a circumflex, the expression matches any character
       except \"new-line\" and the characters in the string.  For
       example, \"[xyz]\" matches \"xx\" and \"zyx\", while \"[!xyz]\
       matches \"abc\" but not \"axb\".  A range of characters may be
       specified by two characters separated by \"-\".  Note that,
       [a-z] matches alphabetics, while [z-a] never matches.
The concatenation of regular expressions is a regular expression.

*/

#define LMAX	512
#define PMAX	256
#define CHAR	1
#define BOL		2
#define EOL		3
#define ANY		4
#define CLASS	5
#define NCLASS	6
#define STAR	7
#define PLUS	8
#define MINUS	9
#define ALPHA	10
#define DIGIT	11
#define NALPHA	12
#define PUNCT	13
#define RANGE	14
#define ENDPAT	15

#define STARTE	16
#define ENDE	17
#define HAT     18
#define QUOTE   19
#define NEWSTAR	20


static char	*pp;
/* static char	lbuf[LMAX];*/
static char	pbuf[PMAX];
static char	*lbuf;


static char *pmatch(char *line, char *pattern);
static char *cclass(char *source, char *src);
static int store(char op);

/*****************************************************************************
	FUNCTION: strdelete() 
	PURPOSE : Delete characters 
*****************************************************************************/
static int strdelete( char *dest, int len ) 
{
    char *p = dest ;
	for ( ; *p ; p++)
	   *p = * (p+len);
	return (0);
}


/*******************************************************
 *   Match the current line (in lbuf[]), return 1 if it does.
 *   RegExpCompile first
 */

int RegExpMatch( char *str )
{
    lbuf = str;
      if (pmatch(str, pbuf))
	     return(1);
   return(0);
}




/*******************************************************
 * RegExpPreprocess 
 * prepresess reg-exp: change spec chars to more specials!
 */

void RegExpPreprocess (char  *source)
{
   register char  *s;	      /* Source string pointer	   */
   register char  *lp;	      /* Last pattern pointer	   */
   register int   c;	      /* Current character	   */
   int		  o;	      /* Temp			   */
   char 	  *spp;       /* Save beginning of pattern */

   s = source;
   lp = pp = pbuf;		/* was  pp = pbuf; */
   
   while (c = *s)
     {
      switch(c)
        {
         //case '+':            /* *':*/
            //*s = NEWSTAR;
            //break;
         //case '@':			  /* '+'*/
            //*s = PLUS;
            //break;
         case '$':
            *s = EOL;
            break;
         case '.':			/* '.':*/
            *s = ANY;
            break;
         case '[':
            *s = STARTE;
            break;
         case ']':
            *s = ENDE;
            break;
         case '\\':
            *s = QUOTE;
			s++;
            break;
         case '*':          	/* added */
            *s = STAR;	
            break;
         case '^':
			 //if (s==source)
			//	*s = BOL;
            if (s>source && *(s-1)==STARTE)
               *s = HAT;
            break;
//         case '^':
//            if (s>source && *(s-1)==STARTE)
//               *s = HAT;
//            break;
        }
       s++;
    }
}







/*******************************************************
 * Compile the pattern into global pbuf[]
 * source = Pattern to compile
 */
int RegExpCompile(char  *source)
{
   register char  *s;	      /* Source string pointer	   */
   register char  *lp;	      /* Last pattern pointer	   */
   register int   c;	      /* Current character	   */
   int		  o;	      /* Temp			   */
   char 	  *spp;       /* Save beginning of pattern */

   s = source;
   lp = pp = pbuf;		/* was  pp = pbuf; */
   
/*   strinsert (source, "!" ,200);*/
//	if (!store(BOL))			/* jpj Beg Of Line always, see eol at the end */
//	   return(0);
/* 	strcat (source, "$");	*/	/* Eol always */
/*   if (debug)*/
/*      printf("Pattern = \"%s\"\n", s);*/

   while (c = *s++) 
     {
      /*
       * STAR, PLUS and MINUS are special.
       */
	
      if (c == NEWSTAR)		/* New star is ".+" */
        {            
         lp = pp;	       /* Remember start       */
	     if (!store(ANY))
	       return(0);
	     if (!store(ENDPAT))
	       return(0);
	    if (!store(ENDPAT))
  	       return(0);
	     spp = pp;		 /* Save pattern end	 */
	     while (--pp > lp)	 /* Move pattern down	 */
   	        *pp = pp[-1];	 /* one byte		 */
	     *pp = STAR;
	     pp = spp;		 /* Restore pattern end  */
	     continue;
	    }
      	 
      if (c == STAR || c == PLUS) /*  || c == '-') */
        {
  	      if (pp == pbuf ||
	      ( o=pp[-1]) == BOL ||
	        o == EOL ||
	        o == STAR ||
	        o == PLUS ||
	        o == MINUS)
	        return(0); /* badpat("Illegal occurrance op.", source, s);*/
	     if (!store(ENDPAT))
	       return(0);

	    if (!store(ENDPAT))
  	       return(0);

	    spp = pp;		 /* Save pattern end	 */
	    while (--pp > lp)	 /* Move pattern down	 */
   	       *pp = pp[-1];	 /* one byte		 */
	    *pp = c; /*  (c == STAR) ? STAR : PLUS; // (c == '-') ? MINUS : PLUS;*/
	    pp = spp;		 /* Restore pattern end  */
	    continue;
       }
      /*
       * All the rest.
       */
      lp = pp;	       /* Remember start       */
      switch(c) {
      case '^':
	     if (!store(BOL))
	        return(0);
	        break;

            case EOL: /* '$':*/
	            if (!store(EOL))
	                return(0);

	            break;
            case ANY: /* '.':*/
	            if (!store(ANY))
	                return(0);
	            break;
            case STARTE : /* '[':*/

	            if (!(s = cclass(source, s)))
	                return(0);
	 
	            break;

            case QUOTE: /* '\\':*/
	            if (*s)
	                c = *s++;
            default:
	            if (!store(CHAR))
	                return(0);
	            if (!store(tolower(c)))
	                return(0);
           }
   }


//   if (!store(EOL))			/* jpj: End Of Line always */
//	   return(0);

   if (!store(ENDPAT))
     return(0);
   if (!store(0))		    /* Terminate string     */
      return(0);
   return(1);
}


/*******************************************************
 *
 * Cut the string at first wild char character.
 */
int	RegExpStrip(char *ss)
{
    char *s = ss; 
	while (*s)
	  {
	   if (*s==QUOTE) /* '\\')*/
	     {
	      strdelete(s, 1);
	      /*s++;*/
	     }
       else if (*s == STAR )
         {
          if (s!=ss) 
             *(s-1)='\0';
          else
             *s = '\0';
          return(1);
         }
	   else
	     {
	      /*if (  *s=='.' || *s=='$' // *s=='!' || *s=='$' ||  *s==':' ||   *s == '-' ||*/
	      	  /*|| *s == '[' || *s== ']' ||*/
	      	  /**s== '*' || *s== '+')*/
	      if (  *s==ANY || *s==EOL /* *s=='!' || *s=='$' ||  *s==':' ||   *s == '-' ||*/
	      	   || *s== ENDE ||
	      	  *s== STARTE || *s== PLUS || *s== NEWSTAR)
	        {
	         *s='\0';
	         return(1);
	        }
	     }
	   s++;
	  }
	return(1);
}


/*******************************************************
 * Does the string contain wildcards ?
 */
int	IsRegExp(char *s)
{
	while (*s)
	  {
	   if (*s=='\\')
	      s++;
	   else
	     {
	      if ( *s=='?' || /*  *s=='!' || *s=='$' ||  || *s == '-'  || *s==':' ||  */
	      		*s == '[' || *s== ']' || *s== '*' || *s== '+' || *s== '@')
	         return(1);
	     }
	   s++;
      }
	return(0);
}

/*******************************************************
 * Compile a class (within [])
 * Pattern start -- for error msg.
 * Class start
 */
static char *cclass(char *source, char *src)
{
   register char   *s;	      /* Source pointer    */
   register char   *cp;       /* Pattern start	   */
   register int    c;	      /* Current character */
   int			   o;	      /* Temp		   */
   s = src;
   o = CLASS;
   if (*s == HAT)   /* '!') */
     {
      ++s;
      o = NCLASS;
     }
   if (!store(o))
      return(NULL);
   cp = pp;
   if (!store(0))			      /* Byte count	 */
      return(NULL);
   while ((c = *s++) && c!=ENDE) /* ']') */
     {
      if (c == QUOTE) /* '\\') */
        {        /* Store quoted char    */
	     if ((c = *s++) == '\0')  /* Gotta get something  */
	        return(NULL); /* badpat("Class terminates badly", source, s);*/
	     else	 
	        {
	         if (!store(tolower(c)))
	            return(NULL);
	        }
        }
      else if (c == '-' &&
	    (pp - cp) > 1 && *s != ENDE && *s != '\0')  /*']' */
	    {
	 	 c = pp[-1];		 /* Range start     */
	 	 pp[-1] = RANGE;	 /* Range signal    */
	 	 if (!store(c))		 /* Re-store start  */
	 	   return(NULL);
	 	 c = *s++;		 /* Get end char and*/
	 	 if (!store(tolower(c)))	 /* Store it	    */
	 	   return(NULL);
        }
      else 
        {
	     if (!store(tolower(c)))	 /* Store normal char */
	        return(NULL);
      	}
   	 }
   if (c != ENDE) /* ']')*/
      return(NULL); /* badpat("Unterminated class", source, s);*/
   if ((c = (pp - cp)) >= 256)
      return(NULL); /* badpat("Class too large", source, s);*/
   if (c == 0)
      return(NULL); /* badpat("Empty class", source, s);*/
   *cp = c;
   return(s);
}

/*******************************************************/

static int store(char op)
{
   if (pp >= &pbuf[PMAX])      
      return(0); /*error("Pattern too complex\n");*/
   *pp++ = op;
   return(1);
}


/*******************************************************/
 /* line: (partial) line to match      */
 /* pattern (partial) pattern to match   */

static char *pmatch(char *line, char *pattern)
{
   register char   *l;	      /* Current line pointer	      */
   register char   *p;	      /* Current pattern pointer      */
   register char    c;	      /* Current character	      */
   char 	   	   *e;	      /* End for STAR and PLUS match  */
   int		   	   op;	      /* Pattern operation	      */
   int		   	   n;	      /* Class counter		      */
   char 	      *are;       /* Start of STAR match	      */
   l = line;
#if 0
   if (debug > 1)
      printf("pmatch(\"%s\")\n", line);
#endif
   p = pattern;
   while ((op = *p++) != ENDPAT) {
#if 0
      if (debug > 1)
	    printf("byte[%d] = 0%o, '%c', op = 0%o\n",
	       l-line, *l, *l, op);
#endif
      switch(op) {
      case CHAR:
	 if (tolower(*l) != *p++)
	    return(0);
	 l++;
	 break;
      case BOL:
	    if (l != lbuf)
  	       return(0);
	    break;
      case EOL:
	 if (*l != '\0')
	    return(0);
	 break;
      case ANY:
	 //if (*l == '\n')			/* jpj testing, no match newline*/
//	 {
		 //l++;
	    //return(0);
	 //}
	 if (*l++ == '\0')
	    return(0);
	 break;
      case DIGIT:
	 if ((c = *l++) < '0' || (c > '9'))
	    return(0);
	 break;
      case ALPHA:
	 c = tolower(*l);
	 l++;
	 if (c < 'a' || c > 'z')
	    return(0);
	 break;
      case NALPHA:
	 c = tolower(*l);
	 l++;
	 if (c >= 'a' && c <= 'z')
	    break;
	 else if (c < '0' || c > '9')
	    return(0);
	 break;
      case PUNCT:
	 c = *l++;
	 if (c == 0 || c > ' ')
	    return(0);
	 break;
      case CLASS:
      case NCLASS:
	 c = tolower(*l);
	 l++;
	 n = *p++ & 0377;
	 do {
	    if (*p == RANGE) {
	       p += 3;
	       n -= 2;
	       if (c >= p[-2] && c <= p[-1])
		  break;
	    }
	    else if (c == *p++)
	       break;
	 } while (--n > 1);
	 if ((op == CLASS) == (n <= 1))
	    return(0);
	 if (op == CLASS)
	    p += n - 2;
	 break;
      case MINUS:
	 e = pmatch(l, p);	 /* Look for a match	*/
	 while (*p++ != ENDPAT); /* Skip over pattern	*/
	 if (e) 		 /* Got a match?	*/
	    l = e;		 /* Yes, update string	*/
	 break; 		 /* Always succeeds	*/
      case PLUS:		 /* One or more ...	*/
	 if ((l = pmatch(l, p)) == 0)
	    return(0);		 /* Gotta have a match	*/
      case STAR:		 /* Zero or more ...	*/
	 are = l;		 /* Remember line start */
	 while (*l && (e = pmatch(l, p)))
	    l = e;		 /* Get longest match	*/
	 while (*p++ != ENDPAT); /* Skip over pattern	*/
	 while (l >= are) {	 /* Try to match rest	*/
	    if (e = pmatch(l, p))
	       return(e);
	    --l;		 /* Nope, try earlier	*/
	 }
	 return(0);		 /* Nothing else worked */
      default:
	 /*printf("Bad op code %d\n", op);*/
	 return(0); /* error("Cannot happen -- match\n");*/
      }
   }
   return(l);
}

/* Compatibility interface for Unix */

char *re_comp (const char * string)
{
	char str[500];
	strncpy(str, string, 499);
	RegExpPreprocess (str);
	if (!RegExpCompile(str))
		return(NULL);
	return (pbuf);
}

int re_exec  (const char * string)
{
	return(RegExpMatch((char *)string));
}
