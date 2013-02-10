#include "print.h"

/*Routines for pretty-printing Rhyming Dictionary output.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

int screenWidth() {
  struct winsize size;
  int MINIMUM = 24;

  if (isatty(1) == 0)
    return 80;
  else {
    if (ioctl(1, TIOCGWINSZ, (char *)&size) < 0)
      return 80;
    else
      return (size.ws_col > MINIMUM) ? size.ws_col : MINIMUM;
  }
}

int countdigits(int i) {
  if (i <= 9) {
    return 1;
  } else {
    return 1 + countdigits(i / 10);
  }
}

struct RhymeWordRow *addToTree(struct RhymeWordRow *tree,
                              struct RhymeWord *word) {
  struct RhymeWordRow *row;

  if (tree == NULL) { /*if there is no tree, make one*/
    row = (struct RhymeWordRow *)testalloc(sizeof(struct RhymeWordRow));

    row->list = NULL;

    pushWord(&row->list, word);

    row->syllables = word->syllables;

    row->greater = NULL;
    row->lesser = NULL;

    return row;
  } else {  /*if there is a tree*/
    if (tree->syllables == word->syllables) { /*if the syllables match*/
      pushWord(&tree->list, word);            /*push the word on the stack*/
    } else if (tree->syllables > word->syllables) {
      /*add to the lesser tree if the word is below the current syllables*/
      tree->lesser = addToTree(tree->lesser, word);
    } else {
      /*otherwise, add to the greater tree*/
      tree->greater = addToTree(tree->greater, word);
    }

    /*and, finally, return the existing tree*/
    return tree;
  }
}

struct RhymeWordList *reverseRhymeWordList(struct RhymeWordList *head) {
  struct RhymeWordList *toreturn = NULL;

  while (head != NULL) {
    pushWord(&toreturn, popWord(&head));
  }

  return toreturn;
}


void printRhymeTree(struct RhymeWordRow *tree, int screenwidth) {
  if (tree == NULL) {
    return;
  } else {
    printRhymeTree(tree->lesser, screenwidth);
    tree->list = reverseRhymeWordList(tree->list);
    printRhymeRow(tree->list, screenwidth);
    printRhymeTree(tree->greater, screenwidth);
  }
}

void printRhymeRow(struct RhymeWordList *list, int screenwidth) {
  struct RhymeWordList *current;
  struct RhymeWordList *next;
  int sent = 0;
  int wordwidth;
  int firstword = 1;

  int startrowwidth = strlen(STARTROWSTRING);
  int indentrowwidth = strlen(INDENTROWSTRING);
  int padwidth = strlen(PADSTRING);

  if (list == NULL)
    return;

  current = list;

  //fprintf(stdout, "%d%s", current->word->syllables, STARTROWSTRING);
  sent += (countdigits(current->word->syllables), startrowwidth);

  while (current != NULL) {
    next = current->next;
    wordwidth = current->word->length;
    if ((sent + wordwidth + padwidth) <= screenwidth) {
      if (!firstword) {
        //fprintf(stdout, "%s", PADSTRING); 
		fprintf(stdout, " ");
        printWord(current->word, stdout);
        sent += (wordwidth + padwidth);
      } else {
        firstword = 0;
        printWord(current->word, stdout);
        sent += (wordwidth + indentrowwidth);
      }
    } else {
      firstword = 0;
      //fprintf(stdout, "\n%s", INDENTROWSTRING);
	  fprintf(stdout, " ");
      printWord(current->word, stdout);
      sent = (indentrowwidth + wordwidth);
    }
    /*printf("%s - %d\n", current->word, current->syllables);*/
    current = next;
  }

  //fprintf(stdout, "\n\n");
  fprintf(stdout, " ");
}

void freeRhymeTree(struct RhymeWordRow *tree) {
  if (tree == NULL) {
    return;
  } else {
    freeRhymeTree(tree->lesser);
    freeRhymeTree(tree->greater);
    freeWordList(tree->list);
    free(tree);
  }
}

void freeOnlyRhymeTree(struct RhymeWordRow *tree) {
  if (tree == NULL) {
    return;
  } else {
    freeOnlyRhymeTree(tree->lesser);
    freeOnlyRhymeTree(tree->greater);
    free(tree);
  }
}

void prettyPrintRhymes(struct RhymeWordList *head) {
  struct RhymeWordRow *tree = NULL;
  struct RhymeWordList *current;

  for (current = head ; current != NULL ; current = current->next) {
    tree = addToTree(tree, current->word);
  }
  
  printRhymeTree(tree, screenWidth());

  freeOnlyRhymeTree(tree);
}

void printFindingWord(struct RhymeWord *word) {
  /*printf("Finding perfect rhymes for ");
  printWord(word, stdout);
  printf("...\n");*/
}

void printNotFound(datum d) {
  /*int i;

  printf("*** Word \"");
  for (i = 0 ; i < d.dsize ; i++) {
    putchar(tolower(d.dptr[i]));
  }
  printf("\" wasn\'t found\n");*/
}

void printInteractiveHelp(FILE *stream) {
  fprintf(stream, " - exits on an empty line\n");
  fprintf(stream, 
          " - type %s to toggle between rhyme and syllable searches\n",
          SWAPSYLLABLEKEY);
  fprintf(stream,
          " - type %s to toggle between merged and non-merged results\n",
          SWAPMERGEDKEY);
  fprintf(stream, " - type %s for help\n", HELPKEY);
}
