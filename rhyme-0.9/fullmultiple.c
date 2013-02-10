#include "longline.h"
#include "dict.h"

/*An application for finding all the multiple pronunciations in the
  CMU pronouncing dictionary.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

int main(int argc, char *argv[]) {
  char *string;
  struct WordEntry *entry;
  struct WordEntryStack *stack = NULL;

  for (string = readLongLine(stdin); string != NULL;
       string = readLongLine(stdin)) {
    entry = stringToWordEntry(string);
    if (isMultiplePronunciation(entry)) {
      addMultiplePronunciation(stack, entry, 0);
    }
    pushWordEntry(&stack, entry);
  }

  printMultiples(stdout, stack);

  return 0;
}
