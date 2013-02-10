#include <stdio.h>
#include <stdlib.h>
#include <readline.h>
#include <history.h>
#include <gdbm.h>

/*A program for viewing Rhyming Dictionary pronunciations.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

#define PRONOUNCE_PROMPT "PRONOUNCE> "

void fatalError(char s[]);

/*returns FOUND if the word is found properly and printed.
  returns NOT_FOUND if the word is not found and an error is needed*/
int printPronunciation(datum word, GDBM_FILE pronouncefile);
