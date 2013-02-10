#include "pronounce.h"
#include "rhyme.h"
#include "alloc.h"

/*A program for viewing Rhyming Dictionary pronunciations.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

void fatalError(char s[]) {
  fprintf(stderr, s);
  exit(1);
}

int printPronunciation(datum word, GDBM_FILE pronouncefile) {
  datum pronunciation;
  int i;

  pronunciation = gdbm_fetch(pronouncefile, word);

  if (pronunciation.dptr != NULL) {
    for (i = 0 ; i < pronunciation.dsize ; i++) {
      putchar(pronunciation.dptr[i]);
    }
    putchar('\n');
    free(pronunciation.dptr);
    /*add_history(line);*/
    return FOUND;
  } else {
    return NOT_FOUND;
  }
}

int main(int argc, char *argv[]) {
  char *line;
  datum word;
  int i;
  struct RhymeWordList *pronunciations;
  struct RhymeWordList *head;
  int found = 0;

  GDBM_FILE pronouncefile,multiplefile;

  if (argc != 3) {
    fprintf(stderr, "*** Usage: %s <pronounce file> <full multiple file>\n", 
	    argv[0]);
    exit(1);
  }

  pronouncefile = gdbm_open(argv[1], 0, GDBM_READER, 0, fatalError);
  multiplefile = gdbm_open(argv[2], 0, GDBM_READER, 0, fatalError);

  using_history();
  rl_bind_key('\t', rl_insert);

  for (line = readline(PRONOUNCE_PROMPT) ; 
       ((line != NULL) && (strlen(line) > 0)) ;
       free(line),line = readline(PRONOUNCE_PROMPT)) {
    word.dptr = (char *)testalloc(strlen(line) + 1);
    word.dsize = strlen(line);
    
    for (i = 0 ; i < word.dsize ; i++) {
      word.dptr[i] = toupper(line[i]);
    }

    pronunciations = getPronunciations(word, multiplefile);

    for (head = pronunciations ; head != NULL ; head = head->next) {
      found = printPronunciation(wordToDatum(head->word), pronouncefile);
    }

    freeWordList(pronunciations);

    if (found == FOUND) {
      add_history(line);
    } else {
      fprintf(stderr, "*** Word \"%s\" wasn't found\n", line);
    }
  }

  gdbm_close(multiplefile);
  gdbm_close(pronouncefile);

  return 0;
}
