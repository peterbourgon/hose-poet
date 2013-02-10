#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <gdbm.h>

/*Primary rhyming dictionary routines.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

#define SWAPSYLLABLEKEY "."
#define SWAPMERGEDKEY ","
#define HELPKEY "?"

#define PROMPT_RHYME "RHYME> "
#define PROMPT_MERGED "RHYME-MERGED> "
#define PROMPT_SYLLABLE "SYLLABLE> "

/*This is a simple true/false value for the isWord() function to return.*/
enum {NOT_FOUND, FOUND};

struct RhymeWord {
  char *word;
  int length;
  int syllables;
};

struct RhymeWordList {
  struct RhymeWord *word;
  struct RhymeWordList *next;
};

struct RhymeWordRow {
  struct RhymeWordList *list;
  int syllables;
  struct RhymeWordRow *lesser;
  struct RhymeWordRow *greater;
};

/*Builds a datum structure from the RhymeWord.
  There is no need to free() the datum's .dptr string so long as
  freeWord is called eventually (after the datum's last use!)*/
datum wordToDatum(struct RhymeWord *word);

/*Converts a RhymeWord to a standard string, which must be free()ed later*/
char *wordToString(struct RhymeWord *word);

/*Builds a RhymeWord structure from the given data.*/
struct RhymeWord *buildWord(char *word, int length, int syllables);

/*Prints the word in RhymeWord to the given stream.
  Use this instead of standard C string printing functions.*/
void printWord(struct RhymeWord *rhymeword, FILE *stream);

/*This is like strcmp, but for RhymeWords.
  Return positive if word1 is larger, negative if word2 is larger
  and 0 if the words are equal*/
int cmpWords(struct RhymeWord *word1, struct RhymeWord *word2);

/*Frees the space allocated by buildWord.*/
void freeWord(struct RhymeWord *rhymeword);

/*Frees the entire stack, including the sub-Word structures.*/
void freeWordList(struct RhymeWordList *head);

/*This is a debugging function*/
void printWordList(struct RhymeWordList *head, FILE *stream);

/*pushWord and popWord provide stack-like facilities for the RhymeWordList
  structure.*/
void pushWord(struct RhymeWordList **stack, struct RhymeWord *word);
struct RhymeWord *popWord(struct RhymeWordList **stack);

/*This parses out a single string into a RhymeWordList stack.*/
struct RhymeWordList *parseWordList(char *list, int start, int length);

/*Returns index of the first space in the datum string.*/
int findSpace(datum d);

/*these two functions pull the word or syllable data from the
  words.db gdbm datafile - I've multiplexed them to save 10 megs*/
struct RhymeWord *keyFromWord(datum d);
int syllablesFromWord(datum d);

/*Returns 1 if the word exists, 0 if the word does not*/
int isWord(datum word, GDBM_FILE wordfile);

/*This takes the word in the given datum and finds its syllable count
  or -1 if the word is not found.*/
int countSyllables(datum word, GDBM_FILE wordfile);

/*This assigns a syllable count to all the words in the given list.*/
void assignSyllables(struct RhymeWordList *head, GDBM_FILE wordfile);

/*As expected, this returns the largest syllable count in the list
  (or 0 if the list is empty).*/
int maxSyllables(struct RhymeWordList *head);

/*And, this returns the smallest syllable count in the list
  (or MAX_INT if the list is empty.*/
int minSyllables(struct RhymeWordList *head);

/*This takes a raw, capitalized word and returns a list of its singular
  rhymes (i.e. a straight word->key->rhymes->list mapping)*/
struct RhymeWordList *getRhymes(datum word, GDBM_FILE wordfile,
				GDBM_FILE rhymefile);

/*This takes a raw, capitalized word and returns a list of all its
  pronunciations (and a word with only one pronunciation gets returned
  without adjustment)*/
struct RhymeWordList *getPronunciations(datum word, GDBM_FILE multiplefile);

/*This merges the two lists into a single list, freeing any duplicates
  in the process.*/
struct RhymeWordList *mergeLists(struct RhymeWordList *list1,
				 struct RhymeWordList *list2);

/*The following four functions return 1 if a word is found and
  0 if no word is found matching the given one.*/

/*This pretty-prints the syllable count for the given word.*/
int displaySyllables(datum word, GDBM_FILE wordfile, GDBM_FILE multiplefile);

/*This pretty-prints the individual results for all the rhymes of the given
  word.*/
int displayRhymes(datum word, GDBM_FILE wordfile, GDBM_FILE rhymefile,
		  GDBM_FILE multiplefile);

/*This pretty-prints the merged results for all the rhymes of the given
  word.*/
int displayMergedRhymes(datum word, GDBM_FILE wordfile, GDBM_FILE rhymefile,
			GDBM_FILE multiplefile);

/*This takes a word plus the flags and chooses among the three
  display functions.*/
int displayResult(char *word, int flags, GDBM_FILE wordfile,
		  GDBM_FILE rhymefile, GDBM_FILE mulitplefile);
