/*____________________________________________________________________________________________

	 Kielikone Oy
	 
	 
	 		RegExp search functions 
	
 _____________________________________________________________________________________________
*/


/* Compatibility to UNIX */

char *re_comp (const char * string);
int re_exec  (const char * string);


/*******************************************************
 *   Match the current line (in lbuf[]), return 1 if it does.
 *   RegExpCompile first
 */
int RegExpMatch( char *str );


/*******************************************************
 * Preprocess 
*/
void RegExpPreprocess (char  *source);

/*******************************************************
 * Compile the pattern into global pbuf[]
 * source = Pattern to compile
 * Automatically add BegOfLine at the beginning and EndOfLine at the End
 */

int RegExpCompile(char  *source);

/*******************************************************
 * Cut the string at first wild char character.
 */
int	RegExpStrip(char *s);

/*******************************************************
 * Does the string contain wildcards ?
 */
int	IsRegExp(char *s);
