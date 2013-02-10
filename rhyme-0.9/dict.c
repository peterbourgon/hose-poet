#include "dict.h"
#include "parts.h"
#include "longline.h"
#include "alloc.h"

/*Routines for parsing the CMU pronouncing dictionary file.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

struct WordEntry *stringToWordEntry(char *string) {
  struct WordEntry *toreturn;
  struct StringPart *parts;
  struct StringPart *current;

  parts = getStringParts(string, 0, strlen(string));

  if (countStringParts(parts) < 2) return NULL;

  toreturn = (struct WordEntry *)testalloc(sizeof(struct WordEntry));

  toreturn->word = parts;
  toreturn->phonemes = parts->next;
  toreturn->rhymekey = NULL;

  toreturn->syllables = 0;

  for (current = parts->next ; current != NULL ;
       current = current->next) {
    if (isSyllable(current)) {
      toreturn->syllables++;
      if (isPrimaryStress(current))
	toreturn->rhymekey = current;
    }
  }

  /*words without primary keys are dropped entirely
  if (toreturn->rhymekey == NULL) {
    free(toreturn);
    return NULL;
    }*/

  toreturn->alternate = NULL;

  toreturn->string = string;

  return toreturn;
}

int isMultiplePronunciation(struct WordEntry *entry) {
  if (entry->word->string[entry->word->length - 1] == ')')
    return 1;
  else
    return 0;
}

void printWordEntry(FILE *stream, struct WordEntry *entry) {
  int i;

  struct StringPart *current;

  if (entry == NULL) {
    fprintf(stream, "<BLANK>\n");
    return;
  }

  for (i = 0 ; i < entry->word->length ; i++)
    fputc(entry->word->string[i], stream);

  fputc(' ', stream);

  for (current = entry->rhymekey; current->next != NULL; 
       current = current->next) {
    for (i = 0 ; i < current->length ; i++)
      fputc(current->string[i], stream);
    fputc('-', stream);
  }

  for (i = 0 ; i < current->length ; i++)
    fputc(current->string[i], stream);

  fprintf(stream, " %d\n", entry->syllables);
}

void freeWordEntry(struct WordEntry *entry) {
  free(entry->string);
  free(entry);
}

void pushWordEntry(struct WordEntryStack **stack, struct WordEntry *entry) {
  struct WordEntryStack *head;

  head = (struct WordEntryStack *)testalloc(sizeof(struct WordEntryStack));

  head->entry = entry;
  head->next = *stack;

  *stack = head;
}

struct WordEntry *popWordEntry(struct WordEntryStack **stack) {
  struct WordEntryStack *head;
  struct WordEntry *toreturn;

  head = *stack;

  if (head == NULL) return NULL;

  *stack = head->next;
  
  toreturn = head->entry;
  free(head);
  return toreturn;
}

struct WordEntryStack *reverseWordEntryStack(struct WordEntryStack *stack) {
  struct WordEntryStack *toreturn = NULL;

  while (stack != NULL) {
    pushWordEntry(&toreturn, popWordEntry(&stack));
  }

  return toreturn;
}

void freeWordEntryStack(struct WordEntryStack *stack) {
  if (stack == NULL) return;
  else {
    freeWordEntryStack(stack->next);
    freeWordEntry(stack->entry);
    free(stack);
  }
}

struct WordEntryTree *addToEntryTree(struct WordEntryTree *tree,
				     struct WordEntry *entry) {
  struct WordEntryTree *newtree;
  int cmp;

  if (tree == NULL) { /*if there is no tree, make one*/
    newtree = (struct WordEntryTree *)testalloc(sizeof(struct WordEntryTree));

    newtree->rhymes = NULL;

    pushWordEntry(&newtree->rhymes, entry);

    newtree->rhymekey = entry->rhymekey;

    newtree->greater = NULL;
    newtree->lesser = NULL;

    return newtree;
  } else {  /*if there is a tree*/
    cmp = cmpStringParts(tree->rhymekey, entry->rhymekey);

    if (cmp == 0) { /*if the keys match*/
      pushWordEntry(&tree->rhymes, entry);    /*push the word on the stack*/
    } else if (cmp > 0) {
      /*add to the lesser tree if the word is below the current syllables*/
      tree->lesser = addToEntryTree(tree->lesser, entry);
    } else {
      /*otherwise, add to the greater tree*/
      tree->greater = addToEntryTree(tree->greater, entry);
    }

    /*and, finally, return the existing tree*/
    return tree;
  }
}

void freeOnlyEntryTree(struct WordEntryTree *tree) {
  if (tree == NULL) return;
  else {
    freeOnlyEntryTree(tree->lesser);
    freeOnlyEntryTree(tree->greater);
    free(tree);
  }
}

int isBaseWord(struct WordEntry *entry, struct WordEntry *multiple) {
  int i;

  if (isMultiplePronunciation(entry)) return 0; /*multiples can't be the base*/

  for (i = 0 ; multiple->word->string[i] != '(' ; i++) {
    if (entry->word->string[i] != multiple->word->string[i])
      return 0;
  }

  /*if there's still more of the entry, it's not the base either*/
  if (i == entry->word->length)
    return 1;
  else
    return 0;
}

int addMultiplePronunciation(struct WordEntryStack *stack,
			      struct WordEntry *entry,
			      int skipduplicates) {
  struct WordEntryStack *base;
  struct WordEntry *alternate;

  for (base = stack ; (base != NULL) && (!isBaseWord(base->entry, entry)) ;
       base = base->next) /*do nothing*/;

  /*This returns 0 because even though the word has no base, it should
    still be added to the rhymes.  It is only conflicts with
    existing rhymekeys for multiple pronunciations that we must worry
    about*/
  if (base == NULL) return 1;

  /*Now go to the end of the list, checking for an identical
    rhymekey along the way (to eliminate nasty duplicates)*/

  alternate = base->entry;

  while (alternate->alternate != NULL) {
    /*skip identical alternate rhymes*/
    if (skipduplicates && 
	(cmpStringParts(entry->rhymekey, alternate->rhymekey) == 0)) return 0;
    else alternate = alternate->alternate;
  }

  if (!skipduplicates || 
      cmpStringParts(entry->rhymekey, alternate->rhymekey) != 0) {
    alternate->alternate = entry;
    return 1;
  } else {
    return 0;
  }
}

void printEntryTree(FILE *stream, struct WordEntryTree *tree) {
  int i;
  struct StringPart *current;

  if (tree == NULL) return;
  else {
    printEntryTree(stream, tree->lesser);

    for (current = tree->rhymekey; current->next != NULL; 
	 current = current->next) {
      for (i = 0 ; i < current->length ; i++)
	fputc(current->string[i], stream);
      fputc('-', stream);
    }

    for (i = 0 ; i < current->length ; i++)
      fputc(current->string[i], stream);

    printEntryRow(stream, tree->rhymes);

    fputc('\n', stream);

    printEntryTree(stream, tree->greater);
  }
}

void printEntryRow(FILE *stream, struct WordEntryStack *stack) {
  int i;

  if (stack == NULL) return;
  else {
    printEntryRow(stream, stack->next);

    fputc(' ', stream);
    for (i = 0 ; i < stack->entry->word->length ; i++) {
      fputc(stack->entry->word->string[i], stream);
    }
  }
}

void printWordEntries(FILE *stream, struct WordEntryStack *stack) {
  struct WordEntryStack *head;

  for (head = stack ; head != NULL ; head = head->next) {
    printWordEntry(stream, head->entry);
  }
}

void printMultiples(FILE *stream, struct WordEntryStack *stack) {
  struct WordEntryStack *head;
  struct WordEntry *multiple;
  int i;

  for (head = stack ; head != NULL ; head = head->next) {
    if ((head->entry->alternate != NULL) &&
	!isMultiplePronunciation(head->entry)) {
      for (i = 0 ; i < head->entry->word->length ; i++)
	fputc(head->entry->word->string[i], stream);

      fputc(' ', stream);

      for (i = 0 ; i < head->entry->word->length ; i++)
	fputc(head->entry->word->string[i], stream);

      for (multiple = head->entry->alternate;
	   multiple != NULL ;
	   multiple = multiple->alternate) {
	fputc(' ', stream);
	for (i = 0 ; i < multiple->word->length ; i++)
	  fputc(multiple->word->string[i], stream);
      }
      fputc('\n', stream);
    }
  }
}

/*
int main(int argc, char *argv[]) {
  char *string;
  struct WordEntry *entry;
  struct WordEntryStack *stack = NULL;
  struct WordEntryTree *tree = NULL;

  for (string = readLongLine(stdin); string != NULL ;
       string = readLongLine(stdin)) {
    entry = stringToWordEntry(string);
    if (entry->rhymekey != NULL) {
      if (isMultiplePronunciation(entry)) {
	if (addMultiplePronunciation(stack, entry, 1)) {
	  pushWordEntry(&stack, entry);
	  tree = addToEntryTree(tree, entry);
	}
      } else {
	pushWordEntry(&stack, entry);
	tree = addToEntryTree(tree, entry);
      }
    }
  }

  stack = reverseWordEntryStack(stack);

  printWordEntries(stdout, stack);

  printMultiples(stdout, stack);

  printEntryTree(stdout, tree);*/

  /*
  for (entry = popWordEntry(&stack); entry != NULL ;
       entry = popWordEntry(&stack)) {
    if (isMultiplePronunciation(entry))
      printWordEntry(stdout, entry);
    freeWordEntry(entry);
    }

  return 0;
}
  */
