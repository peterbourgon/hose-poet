#include "longline.h"

/*Routines for reading/writing strings of arbitrary length.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

void printLongLine(FILE *stream, struct StringBlock *block) {
  int i;

  if (block == NULL) {
    fputc('\n', stream);
    return;
  } else {
    for (i = 0 ; i < block->length ; i++) {
      fputc(block->block[i], stream);
    }

    printLongLine(stream, block->next);
  }
}

struct StringBlock *readBlocks(FILE *stream) {
  int c;
  struct StringBlock *block;

  block = (struct StringBlock *)testalloc(sizeof(struct StringBlock));

  block->length = 0;

  /*This reads in characters until \n, EOF or the end of the BLOCKLENGTH
    occurs.  The \n or EOF will trigger a return from the function.
    But if it hits the end of the BLOCKLENGTH, the current block will
    be ended and the next block will be allocated recursively.*/
  while (block->length < BLOCKLENGTH) {
    c = fgetc(stream);

    if (c == EOF) {
      /*if entire line is just EOF, drop it entirely*/
      if (block->length == 0) {
	free(block);
	return NULL;
      } else {
	/*but if the last line has EOF rather than \n,
	  it should be returned rather than dropped*/
	block->next = NULL;
	return block;
      }
    } else if (c == '\n') {
      block->next = NULL;
      return block;
    } else {
      block->block[block->length] = c;
    }

    block->length++;
  }
  
  block->next = readBlocks(stream);
  return block;
}

void freeBlocks(struct StringBlock *block) {
  if (block == NULL) {
    return;
  } else {
    freeBlocks(block->next);
    free(block);
  }
}

int lineLength(struct StringBlock *block) {
  if (block == NULL) {
    return 0;
  } else {
    return block->length + lineLength(block->next);
  }
}

int cmpLongLine(struct StringBlock *a, struct StringBlock *b) {
  int i;

  if ((a == NULL) && (b == NULL)) {
    return 0;
  } else if (b == NULL) {
    return a->block[0];
  } else if (b == NULL) {
    return 0 - a->block[0];
  } else {
    for (i = 0 ; i < a->length && i < b->length ; i++) {
      if (a->block[i] != b->block[i]) {
	return a->block[i] - b->block[i];
      }
    }

    if (a->length > b->length) {
      return a->block[i];
    } else if (b->length > a->length) {
      return -1 * b->block[i];
    } else {
      return cmpLongLine(a->next, b->next);
    }
  }
}

char *buildString(struct StringBlock *block) {
  char *string;
  int length,i,j;
  struct StringBlock *head;

  length = lineLength(block) + 1;
  string = (char *)testalloc(length);
  
  for (head = block,i = 0 ; head != NULL ; head = head->next) {
    for (j = 0 ; j < head->length ; j++,i++) {
      string[i] = head->block[j];
    }
  }

  string[i] = '\0';
  
  return string;
}

char *readLongLine(FILE *stream) {
  struct StringBlock *block;
  char *line;

  block = readBlocks(stream);

  if (block == NULL) {
    return NULL;
  } else {
    line = buildString(block);
    freeBlocks(block);
    return line;
  }
}

struct StringBlock *readStrippedBlocks(FILE *file) {
  struct StringBlock *toreturn;

  toreturn = readBlocks(file);

  if (toreturn == NULL) {
    return toreturn;
  } else if (lineLength(toreturn) == 0) {
    freeBlocks(toreturn);
    return readStrippedBlocks(file);
  } else if (toreturn->block[0] == '#') {
    freeBlocks(toreturn);
    return readStrippedBlocks(file);
  } else {
    return toreturn;
  }
}
