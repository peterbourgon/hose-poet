#include "alloc.h"

/*This is a wrapper around malloc() that kills rhyme unhappily
  if the requested space cannot be allocated.*/
void *testalloc(size_t size) {
  void *toreturn;

  toreturn = (void *)malloc(size);

  if (toreturn == NULL) {
    fprintf(stderr, "Out of memory trying to allocate %d bytes\n", size);
    fprintf(stderr, "Program terminated\n");
    exit(2);
  } else {
    return toreturn;
  }
}
