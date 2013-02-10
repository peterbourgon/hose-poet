#include <string.h>
#include <stdio.h>

/*Routines for parsing the CMU pronouncing dictionary file.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

struct WordEntry {
  char *string;  /*this is the complete string, since the parts do not
		   make copies of it*/

  struct StringPart *word;
  struct StringPart *phonemes;
  struct StringPart *rhymekey;
  int syllables;
  
  struct WordEntry *alternate; /*the alternate pronunciation(s)*/
};

struct WordEntryStack {
  struct WordEntry *entry;
  struct WordEntryStack *next;
};

struct WordEntryTree {
  struct StringPart *rhymekey;
  struct WordEntryStack *rhymes;
  struct WordEntryTree *lesser;
  struct WordEntryTree *greater;
};

struct WordEntry *stringToWordEntry(char *string);

int isMultiplePronunciation(struct WordEntry *entry);

/*returns 1 if the "entry" is the base word for "multiple" and 0 if not
  This is for finding where to string the multiple pronunciations from*/
int isBaseWord(struct WordEntry *entry, struct WordEntry *multiple);

void printWordEntry(FILE *stream, struct WordEntry *entry);

void freeWordEntry(struct WordEntry *entry);

void pushWordEntry(struct WordEntryStack **stack, struct WordEntry *entry);

struct WordEntry *popWordEntry(struct WordEntryStack **stack);

void freeWordEntryStack(struct WordEntryStack *stack);

struct WordEntryStack *reverseWordEntryStack(struct WordEntryStack *stack);

struct WordEntryTree *addToEntryTree(struct WordEntryTree *tree,
				     struct WordEntry *entry);

void freeOnlyEntryTree(struct WordEntryTree *tree);

/*This assumes "entry" is a multiple pronuncation and "stack" is
  all the previously added words, in stack-like order
  set skipduplicates to true to not add identical rhymes

  returns 1 if the word's rhyme isn't present, and 0 if it is*/
int addMultiplePronunciation(struct WordEntryStack *stack,
			     struct WordEntry *entry,
			     int skipduplicates);

/*This is for printing out the complete set of key-rhyme pairs*/
void printEntryTree(FILE *stream, struct WordEntryTree *tree);

/*This prints out an individual stack of rhymes in reverse order
  (since they'll be inserted in stack-like fashion, it's important
  to re-order them for display)*/
void printEntryRow(FILE *stream, struct WordEntryStack *stack);

/*Prints out each individual word entry from the given stack*/
void printWordEntries(FILE *steam, struct WordEntryStack *stack);

/*Prints out the multiple pronunciations from the given stack*/
void printMultiples(FILE *stream, struct WordEntryStack *stack);
