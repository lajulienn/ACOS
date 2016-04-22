#include <cstdio>
#include <cstdlib> /* for malloc, free */
#include <cstring> /* for memmove */
#include <cctype>  /* for isupper, tolower */

#include "libstemmer.h"

const char * progname;
static int pretty = 1;

void stem_file(struct sb_stemmer * stemmer, FILE * f_in, FILE * f_out)
{
#define INC 10
    int lim = INC;
    sb_symbol * b = (sb_symbol *) malloc(lim * sizeof(sb_symbol));

    while(1) {
        int ch = getc(f_in);
        if (ch == EOF) {
            free(b); return;
        }
        {
            int i = 0;
	    int inlen = 0;
            while(1) {
                if (ch == '\n' || ch == EOF) break;
                if (i == lim) {
                    sb_symbol * newb;
		    newb = (sb_symbol *)
			    realloc(b, (lim + INC) * sizeof(sb_symbol));
		    if (newb == 0) goto error;
		    b = newb;
                    lim = lim + INC;
                }
		/* Update count of utf-8 characters. */
		if (ch < 0x80 || ch > 0xBF) inlen += 1;
                /* force lower case: */
                if (isupper(ch)) ch = tolower(ch);

                b[i] = ch;
		i++;
                ch = getc(f_in);
            }

	    {
		const sb_symbol * stemmed = sb_stemmer_stem(stemmer, b, i);
                if (stemmed == NULL)
                {
                    fprintf(stderr, "Out of memory");
                    exit(1);
                }
                else
		{
		    if (pretty == 1) {
			fwrite(b, i, 1, f_out);
			fputs(" -> ", f_out);
		    } else if (pretty == 2) {
			fwrite(b, i, 1, f_out);
			if (sb_stemmer_length(stemmer) > 0) {
			    int j;
			    if (inlen < 30) {
				for (j = 30 - inlen; j > 0; j--)
				    fputs(" ", f_out);
			    } else {
				fputs("\n", f_out);
				for (j = 30; j > 0; j--)
				    fputs(" ", f_out);
			    }
			}
		    }

		    fputs((char *)stemmed, f_out);
		    putc('\n', f_out);
		}
            }
        }
    }
error:
    if (b != 0) free(b);
    return;
}

/** Display the command line syntax, and then exit.
 *  @param n The value to exit with.
 */
static void
usage(int n)
{
    printf("usage: %s [-l <language>] [-i <input file>] [-o <output file>] [-c <character encoding>] [-p[2]] [-h]\n"
	  "\n"
	  "The input file consists of a list of words to be stemmed, one per\n"
	  "line. Words should be in lower case, but (for English) A-Z letters\n"
	  "are mapped to their a-z equivalents anyway. If omitted, stdin is\n"
	  "used.\n"
	  "\n"
	  "If -c is given, the argument is the character encoding of the input\n"
          "and output files.  If it is omitted, the UTF-8 encoding is used.\n"
	  "\n"
	  "If -p is given the output file consists of each word of the input\n"
	  "file followed by \"->\" followed by its stemmed equivalent.\n"
	  "If -p2 is given the output file is a two column layout containing\n"
	  "the input words in the first column and the stemmed eqivalents in\n"
	  "the second column.\n"
	  "Otherwise, the output file consists of the stemmed words, one per\n"
	  "line.\n"
	  "\n"
	  "-h displays this help\n",
	  progname);
    exit(n);
}
