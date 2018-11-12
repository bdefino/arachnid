/*
Copyright (C) 2018 Bailey Defino
<https://bdefino.github.io>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* arachnid search utility */

#ifndef ARACHNID_H
#define ARACHNID_H

enum ARACHNID_FLAGS {
  ARACHNID_CASE_INSENSITIVE = 1
};

#ifndef DEFAULT_BUFLEN
#define DEFAULT_BUFLEN 1048576
#endif

/* return the number of times a key appears in a file */
unsigned long farachnid(FILE *fp, const char *key, size_t key_size,
  int flags, size_t buflen);

/* return the number of times a key appears at a path */
unsigned long parachnid(const char *path, const char *key, size_t key_size,
  int flags, size_t buflen);

/* return the number of times a key appears in a string */
unsigned long sarachnid(const char *string, size_t size, const char *key,
  size_t key_size, int flags);

#endif
