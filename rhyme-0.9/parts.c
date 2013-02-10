#include "parts.h"
#include "alloc.h"

/*Routines for splitting a single string into individual parts.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

struct StringPart *getStringParts(char *s, int start, int length) {
  int i,wordstart;

  struct StringPart *head;

  for (i = start ; (isspace(s[i]) && (i < length)) ; i++)
    /*do nothing, just find the next non-space*/;

  if (i >= length) return NULL;  /*unless the non-space is the end*/

  wordstart = i;

  for (; (!isspace(s[i]) && (i < length)) ; i++)
    /*do nothing, just find the next space*/;

  head = (struct StringPart *)testalloc(sizeof(struct StringPart));

  head->string = &s[wordstart];
  head->length = i - wordstart;
  head->next = getStringParts(s, i, length);
  
  return head;
}

int cmpStringParts(struct StringPart *a, struct StringPart *b) {
  int i;

  if ((a == NULL) && (b == NULL)) {
    return 0;
  } else if (b == NULL) {
    return a->string[0];
  } else if (a == NULL) {
    return 0 - b->string[0];
  } else {
    for (i = 0 ; i < a->length && i < b->length ; i++) {
      if (a->string[i] != b->string[i]) {
	return a->string[i] - b->string[i];
      }
    }

    if (a->length > b->length) {
      return a->string[i];
    } else if (b->length > a->length) {
      return -1 * b->string[i];
    } else {
      return cmpStringParts(a->next, b->next);
    }
  }
}

void printStringParts(struct StringPart *head) {
  int i;

  if (head != NULL) {
    putchar('>');
  
    for (i = 0 ; i < head->length ; i++) {
      putchar(head->string[i]);
    }

    putchar('<');

    printStringParts(head->next);
  }
}

int countStringParts(struct StringPart *head) {
  struct StringPart *current;
  int count;

  for (current = head, count = 0 ; current != NULL ; 
       current = current->next, count++) /*do nothing, just count parts*/;

  return count;
}

char lastChar(struct StringPart *part) {
  return part->string[part->length - 1];
}

int isSyllable(struct StringPart *part) {
  switch(lastChar(part)) {
  case '0': return 1;
  case '1': return 1;
  case '2': return 1;
  default: return 0;
  }
}

int isPrimaryStress(struct StringPart *part) {
  if (lastChar(part) == '1') return 1;
  else return 0;
}

void freeStringParts(struct StringPart *head) {
  struct StringPart *next;

  if (head != NULL) {
    next = head->next;
    free(head);
    freeStringParts(next);
  }
}


/*
int main(int argc, char *argv[]) {
  struct StringPart *entry1;
  struct StringPart *entry2;

  entry1 = getStringParts(argv[1], 0, strlen(argv[1]));
  entry2 = getStringParts(argv[2], 0, strlen(argv[2]));

  printStringParts(entry1);
  printf("\n");
  printStringParts(entry2);
  printf("\n");
  printf("%d\n", cmpStringParts(entry1, entry2));

  freeStringParts(entry1);
  freeStringParts(entry2);

  return 0;
  }*/


