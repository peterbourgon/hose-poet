#include "rhyme.h"
#include "setup.h"
#include "alloc.h"
#include "print.c"

/*Primary rhyming dictionary routines.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

datum wordToDatum(struct RhymeWord *word) {
  datum d;

  d.dptr = word->word;
  d.dsize = word->length;

  return d;
}

char *wordToString(struct RhymeWord *word) {
  char *s;
  int i;

  s = (char *)testalloc(word->length + 1);

  for (i = 0 ; i < word->length ; i++)
    s[i] = word->word[i];

  s[i] = '\0';

  return s;
}

struct RhymeWord *buildWord(char *word, int length, int syllables) {
  struct RhymeWord *rhymeword;
  int i;

  rhymeword = (struct RhymeWord *)testalloc(sizeof(struct RhymeWord));

  /*always copy the word rather than link it...so the string can be freed
    without causing something else to explode*/
  rhymeword->word = (char *)testalloc(sizeof(char) * (length));
  
  /*note the lack of a terminating null - don't toss this in
    standard string routines*/
  for (i = 0 ; i < length ; i++) {
    rhymeword->word[i] = word[i];
  }

  rhymeword->length = length;
  rhymeword->syllables = syllables;

  return rhymeword;
}

void printWord(struct RhymeWord *rhymeword, FILE *stream) {
  int i;

  for (i = 0 ; i < rhymeword->length ; i++) {
    fputc(tolower(rhymeword->word[i]), stream);
  }
}

int cmpWords(struct RhymeWord *word1, struct RhymeWord *word2) {
  int i;

  for (i = 0 ; i < word1->length && i < word2->length ; i++) {
    if (word1->word[i] != word2->word[i]) {
      return word1->word[i] - word2->word[i];
    }
  }

  if (word1->length > word2->length) {
    return word1->word[i];
  } else if (word2->length > word1->length) {
    return -1 * word2->word[i];
  } else {
    return 0;
  }
}

void freeWord(struct RhymeWord *rhymeword) {
  if (rhymeword->word != NULL) {
    free(rhymeword->word);
  }

  free(rhymeword);
}

void freeWordList(struct RhymeWordList *head) {
  if (head == NULL) {
    return;
  } else {
    freeWordList(head->next);
    freeWord(head->word);
    free(head);
  }
}

void printWordList(struct RhymeWordList *head, FILE *stream) {
  if (head != NULL) {
    printWord(head->word, stream);
    fputc('\n', stream);
    printWordList(head->next, stream);
  }
}

void pushWord(struct RhymeWordList **stack, struct RhymeWord *word) {
  struct RhymeWordList *head;

  head = (struct RhymeWordList *)testalloc(sizeof(struct RhymeWordList));

  head->word = word;
  head->next = *stack;

  *stack = head;
}

struct RhymeWord *popWord(struct RhymeWordList **stack) {
  struct RhymeWordList *head;
  struct RhymeWord *toreturn;

  head = *stack;

  if (head == NULL) return NULL;

  *stack = head->next;
  
  toreturn = head->word;
  free(head);
  return toreturn;
}


struct RhymeWordList *parseWordList(char *list, int start, int length) {
  int i;
  struct RhymeWord *word;
  struct RhymeWordList *wordlist;

  if (start >= length) {
    return NULL;
  } else {
    for (i = start ; (!isspace(list[i]) && (i < length)) ; i++)
      /*do nothing, just find the space or end of string*/;
    
    /*build a word, setting the syllables to 0 (for now)*/
    word = buildWord(list + start, (i - start), 0);
  
    wordlist = (struct RhymeWordList *)testalloc(sizeof(struct RhymeWordList));
    wordlist->word = word;
    wordlist->next = parseWordList(list, i + 1, length);

    return wordlist;
  }
}

int findSpace(datum d) {
  int i;

  for (i = 0 ; !isspace(d.dptr[i]) ; i++)
    /*do nothing*/;

  return i;
}

struct RhymeWord *keyFromWord(datum d) {
  int i;

  i = findSpace(d);

  return buildWord(d.dptr, i, 0);
}

int syllablesFromWord(datum d) {
  int i,j,maxlength;
  char *syllables;
  int count;

  /*make a syllable buffer string - it will never be longer than the
    actual string in the datum*/
  syllables = (char *)testalloc(sizeof(char) * (strlen(d.dptr) + 1));

  maxlength = strlen(d.dptr);

  i = findSpace(d);

  for (j = 0,i++ ; (j < maxlength) && (i < d.dsize) ; i++,j++) {
    syllables[j] = d.dptr[i];
  }

  syllables[j] = '\0';

  count = atoi(syllables);
  free(syllables);

  return count;
}

int countSyllables(datum word, GDBM_FILE wordfile) {
  datum wordvalue;
  int syllables;

  wordvalue = gdbm_fetch(wordfile, word);

  if (wordvalue.dptr == NULL) {
    return -1;
  } else {
    syllables = syllablesFromWord(wordvalue);
    free(wordvalue.dptr);
    return syllables;
  }
}

int isWord(datum word, GDBM_FILE wordfile) {
  datum wordvalue;
  
  wordvalue = gdbm_fetch(wordfile, word);

  if (wordvalue.dptr == NULL) {
    return 0;
  } else {
    free(wordvalue.dptr);
    return 1;
  }
}


void assignSyllables(struct RhymeWordList *head, GDBM_FILE wordfile) {
  datum d;

  if (head == NULL) {
    return;
  } else {
    if (head->word != NULL) {
      d.dptr = head->word->word;
      d.dsize = head->word->length;

      head->word->syllables = countSyllables(d, wordfile);
    }

    assignSyllables(head->next, wordfile);
  }
}

struct RhymeWordList *getRhymes(datum word, GDBM_FILE wordfile,
				GDBM_FILE rhymefile) {
  struct RhymeWordList *toreturn = NULL;

  datum key;   /*the first value, the key+syllables, from the base word*/
  struct RhymeWord *parsedkey;  /*the first value, minues the syllables*/
  datum parsedkeydatum;         /*the raw key, suitable for a second fetch*/
  datum value; /*the second value, the word list, from the first key*/

  key = gdbm_fetch(wordfile, word);

  if (key.dptr == NULL) { /*if there is no key, there are no rhymes*/
    return NULL;
  } else {  /*there is a key value, so there should be rhymes*/
    parsedkey = keyFromWord(key);
    parsedkeydatum.dptr = parsedkey->word;
    parsedkeydatum.dsize = parsedkey->length;

    value = gdbm_fetch(rhymefile, parsedkeydatum);

    if (value.dptr != NULL) { /*this should never be NULL unless data is bad*/
      toreturn = parseWordList(value.dptr, 0, value.dsize);

      free(value.dptr);
    }

    free(key.dptr);
    freeWord(parsedkey);

    return toreturn;
  }
}

struct RhymeWordList *getPronunciations(datum word, GDBM_FILE multiplefile) {
  datum pronunciations;
  struct RhymeWord *rhymeword;
  struct RhymeWordList *toreturn;

  pronunciations = gdbm_fetch(multiplefile, word);

  if (pronunciations.dptr == NULL) { /*a single pronunciation*/
    rhymeword = buildWord(word.dptr, word.dsize, 0);
    toreturn = (struct RhymeWordList *)testalloc(sizeof(struct RhymeWordList));
    toreturn->word = rhymeword;
    toreturn->next = NULL;

    return toreturn;
  } else { /*multiple pronunciations*/
    toreturn = parseWordList(pronunciations.dptr, 0, pronunciations.dsize);
    return toreturn;
  }
}

int maxSyllables(struct RhymeWordList *head) {
  int max;

  if (head == NULL) {
    return 0;
  } else {
    max = maxSyllables(head->next); 

    if (head->word->syllables > max) {
      return head->word->syllables;
    } else {
      return max;
    }
  }
}


int minSyllables(struct RhymeWordList *head) {
  int min;

  if (head == NULL) {
    return INT_MAX;
  } else {
    min = minSyllables(head->next);

    if (head->word->syllables < min) {
      return head->word->syllables;
    } else {
      return min;
    }
  }
}


struct RhymeWordList *mergeLists(struct RhymeWordList *list1,
				 struct RhymeWordList *list2) {
  int compare;
  struct RhymeWordList *next;

  if (list1 == NULL) {
    return list2;
  } else if (list2 == NULL) {
    return list1;
  } else {
    compare = cmpWords(list1->word, list2->word);

    if (compare == 0) {
      /*if the words are equal free one and return the other, linked
	to the remaining items*/
      next = list2->next;
      freeWord(list2->word);
      free(list2);
      list1->next = mergeLists(list1->next, next);
      return list1;
    } else if (compare < 0) {  /*if list1 is higher*/
      list1->next = mergeLists(list1->next, list2);
      return list1;
    } else {                   /*if list2 is higher*/
      list2->next = mergeLists(list1, list2->next);
      return list2;
    }
  }
}

int displaySyllables(datum word, GDBM_FILE wordfile, GDBM_FILE multiplefile) {
  int max;
  int min;
  int found = NOT_FOUND;

  struct RhymeWordList *pronunciations;

  if (!isWord(word, wordfile)) {
    printNotFound(word);

    return NOT_FOUND;
  } else {
    pronunciations = getPronunciations(word, multiplefile);

    assignSyllables(pronunciations, wordfile);

    max = maxSyllables(pronunciations);
    min = minSyllables(pronunciations);

    if (min == max) {
      printf("%d %s", max, (max == 1) ? "syllable\n" : "syllables\n");
      found = FOUND;
    } else {
      printf("%d-%d syllables\n", min, max);
      found = FOUND;
    }

    freeWordList(pronunciations);

    return found;
  }
}

int displayRhymes(datum word, GDBM_FILE wordfile, GDBM_FILE rhymefile,
		  GDBM_FILE multiplefile) {
  struct RhymeWordList *pronunciations;
  struct RhymeWordList *head;
  struct RhymeWordList *rhymes;

  if (!isWord(word, wordfile)) {
    printNotFound(word);

    return NOT_FOUND;
  } else {
    pronunciations = getPronunciations(word, multiplefile);

    for (head = pronunciations ;  head != NULL ; head = head->next) {
      printFindingWord(head->word);
      rhymes = getRhymes(wordToDatum(head->word), wordfile, rhymefile);
      assignSyllables(rhymes, wordfile);
      prettyPrintRhymes(rhymes);
      freeWordList(rhymes);
    }

    freeWordList(pronunciations);
    
    return FOUND;
  }
}

int displayMergedRhymes(datum word, GDBM_FILE wordfile, GDBM_FILE rhymefile,
			GDBM_FILE multiplefile) {
  struct RhymeWordList *pronunciations;
  struct RhymeWordList *rhymes = NULL;
  struct RhymeWordList *head;

  if (!isWord(word, wordfile)) {
    printNotFound(word);

    return NOT_FOUND;
  } else {
    pronunciations = getPronunciations(word, multiplefile);

    printFindingWord(pronunciations->word);

    for (head = pronunciations ; head != NULL ; head = head->next) {
      rhymes = mergeLists(rhymes, getRhymes(wordToDatum(head->word),
					    wordfile, rhymefile));
    }

    assignSyllables(rhymes, wordfile);
    prettyPrintRhymes(rhymes);
    freeWordList(rhymes);
    freeWordList(pronunciations);

    return FOUND;
  }
}

int displayResult(char *word, int flags, GDBM_FILE wordfile,
		  GDBM_FILE rhymefile, GDBM_FILE multiplefile) {
  datum d;
  int i;
  int result = 0;

  d.dsize = strlen(word);
  d.dptr = (char *)testalloc(d.dsize + 1);
  
  for (i = 0 ; word[i] != '\0' ; i++)
    d.dptr[i] = toupper(word[i]);
  d.dptr[i] = '\0';

  if (flags & FLAG_SYLLABLE) {
    result = displaySyllables(d, wordfile, multiplefile);
  } else if (flags & FLAG_MERGED) {
    result = displayMergedRhymes(d, wordfile, rhymefile, multiplefile);
  } else {
    result = displayRhymes(d, wordfile, rhymefile, multiplefile);
  }

  free(d.dptr);
  return result;
}
