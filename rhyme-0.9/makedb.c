#include "makedb.h"

/*A program to parse CMU pronouncing dictionary files to rhyming dictionary
  GDBM data files.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

int execCompile(char *filename) {
  int fd[2];
  pid_t pid;

  if (pipe(fd) < 0) {
    fprintf(stderr, "Pipe creation error\n");
    exit(1);
  }

  if ((pid = fork()) < 0) {
    fprintf(stderr, "Fork error\n");
    exit(1);
  } else if (pid > 0) { /*parent*/
    close(fd[0]);
    return fd[1];
  } else {              /*child*/
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    return execl("./compile", "compile", "-", filename, (char *)0);
  }

  return -1;
}

void printOutputToCompile(char *filename, void *toprint,
			  void (*printFunc)(FILE *file,
					    void *toprint)) {
  int fd,returnval;
  pid_t pid;
  FILE *file;

  fd = execCompile(filename);
  file = fdopen(fd, "w");

  (*printFunc)(file, toprint);

  fclose(file);
  close(fd);
  pid = wait(&returnval);
}

int main(int argc, char *argv[]) {
  long int lines;

  char *string;
  struct WordEntry *entry;
  struct WordEntryStack *stack = NULL;
  struct WordEntryTree *tree = NULL;

  if (argc != 4) {
    fprintf(stderr, "*** Usage: makedb <words.db> <rhymes.db> ");
    fprintf(stderr, "<multiple.db>\n");
    exit(1);
  }

  for (string = readLongLine(stdin),lines = 0l; string != NULL;
       string = readLongLine(stdin),lines++) {
    entry = stringToWordEntry(string);
    if (entry->rhymekey != NULL) {
      if (isMultiplePronunciation(entry)) {
	if (addMultiplePronunciation(stack, entry, 1)) {
	  pushWordEntry(&stack, entry);
	  tree = addToEntryTree(tree, entry);
	}
      } else {
	pushWordEntry(&stack, entry);
	tree = addToEntryTree(tree, entry);
      }
    }
  }

  fprintf(stderr, "%ld lines read from standard input\n", lines);

  printOutputToCompile(argv[1], stack, 
		       (void (*)(FILE *, void *))(printWordEntries));

  printOutputToCompile(argv[2], tree, 
		       (void (*)(FILE *, void *))(printEntryTree));

  printOutputToCompile(argv[3], stack, 
		       (void (*)(FILE *, void *))(printMultiples));

  freeOnlyEntryTree(tree);
  freeWordEntryStack(stack);

  return 0;
}
