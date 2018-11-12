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
#include <sys/stat.h>

/* path specification */

#ifndef FSPECIFY_H
#define FSPECIFY_H

enum SPECIFY_FLAGS {
  SPECIFY_BLK_LINKS = 1,
  SPECIFY_BLKS = 2,
  SPECIFY_CHR_LINKS = 4,
  SPECIFY_CHRS = 8,
  SPECIFY_DIR_LINKS = 16,
  SPECIFY_DIRS = 32,
  SPECIFY_FIFO_LINKS = 64,
  SPECIFY_FIFOS = 128,
  SPECIFY_REG_LINKS = 256
};

/* return whether a path is specified */
unsigned int pspecify(const char *path, unsigned int flags);

/* return whether a struct stat * is specified */
unsigned int specify(const struct stat *lst, const struct stat *st,
  unsigned int flags);

#endif
