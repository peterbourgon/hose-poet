#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*Routines for splitting a single string into individual parts.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

struct StringPart {
  char *string;
  int length;
  struct StringPart *next;
};

struct StringPart *getStringParts(char *s, int start, int length);

int cmpStringParts(struct StringPart *a, struct StringPart *b);

void printStringParts(struct StringPart *head);

void freeStringParts(struct StringPart *head);

int countStringParts(struct StringPart *head);

char lastChar(struct StringPart *part);

int isSyllable(struct StringPart *part);

int isPrimaryStress(struct StringPart *part);
