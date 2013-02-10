#include <stdio.h>
#include "alloc.h"

/*Routines for reading/writing strings of arbitrary length.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

#define BLOCKLENGTH 1024

struct StringBlock {
  char block[BLOCKLENGTH];
  int length;
  struct StringBlock *next;
};

/*This prints the entire list of blocks to the given stream.*/
void printLongLine(FILE *stream, struct StringBlock *block);

/*This reads in one or more blocks from the stream, or returns NULL
  if EOF is reached.*/
struct StringBlock *readBlocks(FILE *stream);

/*This frees up all the allocated space in the given blocks.*/
void freeBlocks(struct StringBlock *block);

/*This returns the length of the string.*/
int lineLength(struct StringBlock *block);

/*This is a strcmp for long lines.*/
int cmpLongLine(struct StringBlock *a, struct StringBlock *b);

/*This converts the list of blocks to a normal string.*/
char *buildString(struct StringBlock *block);

/*This returns a long string from the given stream, which must
  be free()ed later on.*/
char *readLongLine(FILE *stream);

/*This is a wrapper around readBlocks that strips out comments
  and blank lines.*/
struct StringBlock *readStrippedBlocks(FILE *file);
