#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "longline.h"
#include "dict.h"
#include "parts.h"

/*A program to parse CMU pronouncing dictionary files to rhyming dictionary
  GDBM data files.
  Copyright (C) 2000,2001,2002 Brian Langenberger
  Released under the terms of the GNU Public License.
  See the file "COPYING" for full details.*/

/*This takes a filename for a GDBM and returns a file handle
  for "compile" to write to.*/
int execCompile(char *filename);
