#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*Routines for pretty-printing Rhyming Dictionary output.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

/*These are for pretty-printing output*/
#define STARTROWSTRING ": "
#define INDENTROWSTRING "   "
#define PADSTRING ", "

int screenWidth();

/*Returns the number of digits in an integer.
  103 has 3 digits, for example*/
int countdigits(int i);

/*This adds a RhymeWord to a binary RhymeWordRow tree, 
  sorting the words by syllable, reverse-sorted (stack-like)*/
struct RhymeWordRow *addToTree(struct RhymeWordRow *tree,
			      struct RhymeWord *word);

/*This reverses the given RhymeWordList in linear time.*/
struct RhymeWordList *reverseRhymeWordList(struct RhymeWordList *head);

/*This pretty-prints the RhymeTree, depth-first-style.*/
void printRhymeTree(struct RhymeWordRow *tree, int screenwidth);

/*This pretty-prints the RhymeWordList in each tree node.*/
void printRhymeRow(struct RhymeWordList *list, int screenwidth);

/*This recursively frees up all the space used by the tree.*/
void freeRhymeTree(struct RhymeWordRow *tree);

/*This only frees the allocated RhymeWordRow structs, *not* the
  internal lists or words themselves.*/
void freeOnlyRhymeTree(struct RhymeWordRow *tree);

/*This takes a list of rhymes returned by getRhymes() and
  pretty-prints it, saving plenty of time in the process.*/
void prettyPrintRhymes(struct RhymeWordList *head);

void printFindingWord(struct RhymeWord *word);

void printNotFound(datum d);

void printInteractiveHelp(FILE *stream);
