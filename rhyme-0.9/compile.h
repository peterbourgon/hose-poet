#include <stdio.h>
#include <stdlib.h>
#include <gdbm.h>
#include <string.h>

/*A program for building GDBM files from space-delimited text files.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

/*Returns the first space in the given string
  (or -1 if none is found).*/
int findSpace(char *s);

/*Returns the first non-space starting at the given offset*/
int findNonSpace(char *s, int start);
