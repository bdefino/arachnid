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

#include "specify.h"

/* path specification */

/* return whether a path is specified */
unsigned int pspecify(const char *path, unsigned int flags) {
  struct stat lst, st;
  
  if (!lstat(path, &lst) && !stat(path, &st))
    return specify(&lst, &st, flags);
  return 0; /* can't get link or target status */
}

/* return whether a struct stat * is specified */
unsigned int specify(const struct stat *lst, const struct stat *st,
    unsigned int flags) {
  if (S_ISLNK(lst->st_mode)) { /* handle symlinks */
    if ((S_ISREG(st->st_mode) && !(flags & SPECIFY_BLK_LINKS))
        || (S_ISDIR(st->st_mode) && !(flags & SPECIFY_DIR_LINKS))
        || (S_ISBLK(st->st_mode) && !(flags & SPECIFY_BLK_LINKS))
        || (S_ISCHR(st->st_mode) && !(flags & SPECIFY_CHR_LINKS))
        || (S_ISFIFO(st->st_mode) && !(flags & SPECIFY_FIFO_LINKS)))
      return 0; /* none of the *_LINKS flags specify the target */
  }
  
  if (S_ISREG(st->st_mode)
      || (S_ISDIR(st->st_mode) && (flags & SPECIFY_DIRS))
      || (S_ISBLK(st->st_mode) && (flags & SPECIFY_BLKS))
      || (S_ISCHR(st->st_mode) && (flags & SPECIFY_CHRS))
      || (S_ISFIFO(st->st_mode) && (flags & SPECIFY_FIFOS)))
    return 1; /* regular file or the SPECIFY_* flags specify the target */
  return 0;
}
