#include "rhyme.h"
#include "setup.h"
#include "print.h"
#include "main.h"
#include <readline.h>
#include <history.h>

/*The primary Rhyming Dictionary application routine.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

char *getWord(int flags) {
  if (flags & FLAG_SYLLABLE) {
    return readline(PROMPT_SYLLABLE);
  } else if (flags & FLAG_MERGED) {
    return readline(PROMPT_MERGED);
  } else {
    return readline(PROMPT_RHYME);
  }
}

int main(int argc, char *argv[]) {
  GDBM_FILE wordfile,rhymefile,multiplefile;
  int flags = 0;
  int wordindex = 0;

  int returnvalue = 0;

  char *word;

  rhymeSetup(&wordfile, &rhymefile, &multiplefile, &flags, argc, argv,
             &wordindex);

  if (flags & FLAG_INTERACTIVE) {
    using_history();
    rl_bind_key('\t', rl_insert);

    printf("Rhyming Dictionary %s - interactive mode\n", VERSION);

    printInteractiveHelp(stdout);

    word = getWord(flags);

    while ((word != NULL) && (strlen(word) > 0)) {
      if (strcmp(SWAPSYLLABLEKEY, word) == 0) {
        swapSyllableModes(&flags);
      } else if (strcmp(SWAPMERGEDKEY, word) == 0) {
        swapMergedModes(&flags);
      } else if (strcmp(HELPKEY, word) == 0) {
        printInteractiveHelp(stdout);
      } else {
        displayResult(word, flags, wordfile, rhymefile, multiplefile);
        add_history(word);
      }

      free(word);

      word = getWord(flags);
    }

    if (word != NULL) free(word);
    clear_history();

  } else {
    returnvalue = 
    !displayResult(argv[wordindex], flags, wordfile, rhymefile, multiplefile);
  }

  gdbm_close(wordfile);
  gdbm_close(rhymefile);
  gdbm_close(multiplefile);
  
  fprintf(stdout, "\n");
  return returnvalue;
}
