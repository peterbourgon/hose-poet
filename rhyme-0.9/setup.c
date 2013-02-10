#include "setup.h"
#include "alloc.h"

/*Useful setup code for the Rhyming Dictionary
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

struct option long_options[] =
{
  {"syllable", no_argument, NULL, 's'},
  {"merged", no_argument, NULL, 'm'},
  {"interactive", no_argument, NULL, 'i'},
  {"version", no_argument, NULL, 'v'},
  {"help", no_argument, NULL, 'h'},
  {0,0,0,0}
};

void fatalError(char s[]) {
  fprintf(stderr, s);
  exit(1);
}

void rhymeSetup(GDBM_FILE *wordfile, GDBM_FILE *rhymefile, 
		GDBM_FILE *multiplefile,
		int *flags, int argc, char *argv[], int *wordindex) {

  int option_index = 0;
  int c;
  char *dir;
  char *env;

  *flags = 0;

  while ((c = getopt_long(argc, argv, "smvhi", long_options,
			  &option_index)) != EOF) {
    switch(c) {
    case 'v':
      printf("Rhyming Dictionary %s\n", VERSION);
      exit(0);
    case 'h':
      printHelp(stderr);
      exit(0);
    case 's':
      *flags |= FLAG_SYLLABLE;
      break;
    case 'i':
      *flags |= FLAG_INTERACTIVE;
      break;
    case 'm':
      *flags |= FLAG_MERGED;
      break;
    }
  }

  *wordindex = optind;

  if ((!(*flags & FLAG_INTERACTIVE)) && (*wordindex >= argc)) {
    printHelp(stderr);
    exit(0);
  }

  env = getenv(RHYME_ENV);

  if (env == NULL)
    dir = DEFAULT_RHYMEPATH;
  else
    dir = env;

  *wordfile = openGDBMFile(dir, WORDS_NAME);
  *rhymefile = openGDBMFile(dir, RHYMES_NAME);
  *multiplefile = openGDBMFile(dir, MULTIPLES_NAME);
}


GDBM_FILE openGDBMFile(char *dir, char *file) {
  /*I *really* hope GDBM doesn't need the pathnames after gdbm_open
    is called.  I should check the GDBM code to make sure nothing 
    bad happens*/

  char *path;
  GDBM_FILE toreturn;

  path = (char *)testalloc((strlen(dir) + strlen(file) + 1) * sizeof(char));
  path[0] = '\0';
  strcat(path, dir);
  strcat(path, file);

  toreturn = gdbm_open(path, 0, GDBM_READER, 0, fatalError);
  
  if (toreturn == NULL) {
    fprintf(stderr, "*** Error for %s: %s\n", path, gdbm_strerror(gdbm_errno));
    exit(1);
  }

  free(path);
  return toreturn;
}

void printHelp(FILE *stream) {
  fprintf(stream, "Usage : rhyme [OPTIONS] <word>\n\n");
  fprintf(stream, "Search type:   (perfect rhyme is default)\n");
  fprintf(stream, "  -s, --syllable\t");
  fprintf(stream, "returns only the number of syllables\n");
  fprintf(stream, "  -m, --merged\t\t");
  fprintf(stream, "multiple pronunciations are merged\n");
  fprintf(stream, "\nMiscellaneous options:\n");
  fprintf(stream, "  -i, --interactive\t");
  fprintf(stream, "interactive mode\n");
  fprintf(stream, "  -h, --help\t\tthis help message\n");
  fprintf(stream, "  -v, --version\t\tthis program's version\n");
  fprintf(stream, "\nEnvironment variables:\n\n");
  fprintf(stream, "%s\t\tthe directory of the database files\n", RHYME_ENV);
  fprintf(stream, "\nThe output is ordered by syllable count and alphabetized\n");
}

void swapSyllableModes(int *flags) {
  if (*flags & FLAG_SYLLABLE) {
    *flags &= ~FLAG_SYLLABLE;
  } else {
    *flags |= FLAG_SYLLABLE;
  }
}

void swapMergedModes(int *flags) {
  if (*flags & FLAG_MERGED) {
    *flags &= ~FLAG_MERGED;
  } else {
    *flags |= FLAG_MERGED;
  }
}
