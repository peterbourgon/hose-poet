#include "compile.h"
#include "longline.h"

/*A program for building GDBM files from space-delimited text files.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

int findSpace(char *s) {
  int i;

  for (i = 0 ; ((s[i] != ' ') && (s[i] != '\0')) ; i++)
    /*Do nothing*/;

  if (s[i] == '\0')
    return -1;
  else
    return i;
}

int findNonSpace(char *s, int start) {
  int i;

  for (i = start ; ((s[i] == ' ') && (s[i] != '\0')) ; i++)
    /*Do nothng*/;

  if (s[i] == '\0')
    return -1;
  else
    return i;
}

int main(int argc, char *argv[]) {
  FILE *input;
  GDBM_FILE output;

  int i;

  char *line;
  int linelength;

  int lines = 0;
  int space,nonspace;

  datum keyd,valued;

  if (argc != 3) {
    fprintf(stderr, 
	    "*** Usage: compile <text input file> <gdbm output file>\n");
    exit(1);
  }

  /*if the first argument is "-", read from stdin*/
  if (strcmp(argv[1], "-") == 0) {
    input = stdin;
  } else {
    input = fopen(argv[1], "r");
  }

  if (input == NULL) {
    fprintf(stderr,
	    "error opening %s for reading\n", argv[1]);
    exit(1);
  }

  output = gdbm_open(argv[2], 0, GDBM_NEWDB, 0644, 0);

  if (output == NULL) {
    fprintf(stderr,
	    "error opening %s for writing\n", argv[2]);
    if (strcmp(argv[1], "-") != 0) fclose(input);
    exit(1);
  }

  for (line = readLongLine(input) ; line != NULL ;
       free(line),line = readLongLine(input)) {

    linelength = strlen(line);
    space = findSpace(line);

    if (space == -1) {
      fprintf(stderr, "Line %d has no space!\n", lines);
      exit(1);
    }

    nonspace = findNonSpace(line, space);

    keyd.dptr = line;
    keyd.dsize = space;

    valued.dptr = line + nonspace;
    valued.dsize = linelength - nonspace;

#ifdef DEBUG
    for (i = 0 ; i < keyd.dsize ; i++) {
      fputc(keyd.dptr[i], stderr);
    }

    fprintf(stderr, "<>");

    for (i = 0 ; i < valued.dsize ; i++) {
      fputc(valued.dptr[i], stderr);
    }

    fputc('\n', stderr);
#endif

    if (gdbm_store(output, keyd, valued, GDBM_INSERT) == 1) {
      fprintf(stderr, "Key ");

      for (i = 0 ; i < keyd.dsize ; i++)
	fputc(keyd.dptr[i], stderr);

      fprintf(stderr, " is already present in database\n");
      exit(1);
    }

    lines++;
  }

  gdbm_sync(output);

  /*only close the file if it is not stdin*/
  if (strcmp(argv[1], "-") != 0) {
    fclose(input);
  }

  gdbm_close(output);

  printf("Inserted %d lines into %s\n", lines, argv[2]);

  return 0;
}
