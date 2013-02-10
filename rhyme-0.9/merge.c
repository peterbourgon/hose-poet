#include "merge.h"
#include "longline.h"

/*A program to merge the lines of two files to standard output.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

FILE *openFile(char *name) {
  FILE *toreturn;

  toreturn = fopen(name, "r");
  if (toreturn == NULL) {
    fprintf(stderr, "Error opening %s for reading\n", name);
    exit(1);
  }

  return toreturn;
}

int main(int argc, char *argv[]) {
  FILE *file1;
  FILE *file2;

  struct StringBlock *block1;
  struct StringBlock *block2;

  int cmp;

  if (argc != 3) {
    fprintf(stderr, "*** Usage %s: <file1> <file2>\n", argv[0]);
    return 1;
  }

  file1 = openFile(argv[1]);
  file2 = openFile(argv[2]);

  for (block1 = readStrippedBlocks(file1), block2 = readStrippedBlocks(file2) ;
       ((block1 != NULL) && (block2 != NULL)) ; /*do nothing*/) {

    cmp = cmpLongLine(block1, block2);

    if (cmp == 0) { /*if the lines are identical*/
      freeBlocks(block2);
      printLongLine(stdout, block1);
      freeBlocks(block1);
      block1 = readStrippedBlocks(file1);
      block2 = readStrippedBlocks(file2);
    } else if (cmp > 0) {
      printLongLine(stdout, block2);
      freeBlocks(block2);
      block2 = readStrippedBlocks(file2);
    } else {
      printLongLine(stdout, block1);
      freeBlocks(block1);
      block1 = readStrippedBlocks(file1);
    }
  }

  if (block1 == NULL) {
    for (; block2 != NULL ; freeBlocks(block2), 
	   block2 = readStrippedBlocks(file2)) {
      printLongLine(stdout, block2);
    }
  } else if (block2 == NULL) {
    for (; block1 != NULL ; freeBlocks(block1), 
	   block1 = readStrippedBlocks(file1)) {
      printLongLine(stdout, block1);
    }
  }

  freeBlocks(block1);
  freeBlocks(block2);

  fclose(file1);
  fclose(file2);

  return 0;
}
