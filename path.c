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

#include "path.h"

/* Pythonic path manipulations */

/* place an absolute path into (optional) dest */
char *abspath(char *dest, const char *src) {
  char *cwd;
  
  if (src == NULL) {
    return NULL;
  } else if (isabspath(src)) {
    if (dest == NULL && (dest = (char *) malloc(strlen(src) + 1)) == NULL)
      return NULL;
    strcpy(dest, src);
    return dest;
  } else if ((cwd = getcwd(NULL, 0)) == NULL) {
    return NULL;
  }
  dest = joinpaths(dest, cwd, src);
  free(cwd);
  return dest; /* NULL on error */
}

/* return a pointer to the base name */
char *basename(const char *src) {
  unsigned int i, srclen;
  
  if (src != NULL && (srclen = strlen(src)) > 0) {
    for (i = srclen - 1; i > 0 && *(src + i) != PATH_SEP; i--)
      ;
    return (char *) src + i;
  }
  return NULL;
}

/* place directory (if available) in (optional) dest */
char *dirname(char *dest, const char *src) {
  int destlen, i;
  
  destlen = strlen(src);
  
  while (*(src + destlen - 1) == PATH_SEP) /* ignore trailing separators */
    destlen--;
  
  while (destlen && *(src + destlen) != PATH_SEP) /* omit final separator */
    destlen--;
  
  if (dest == NULL && (dest = (char *) malloc(destlen + 1)) == NULL)
    return NULL;
  strncpy(dest, src, destlen);
  return dest;
}

/* return a pointer to the extension */
char *extension(const char *src) {
  unsigned int i, srclen;
  
  if (src != NULL && (srclen = strlen(src)) > 0) {
    for (i = srclen - 1; i >= 0 && *(src + i) != '.'; i--) {
      if (*(src + i) == PATH_SEP) /* no extension */
        return NULL;
    }
    return (char *) src + i;
  }
  return NULL;
}

/* return whether the path is absolute */
unsigned int isabspath(const char *path) {
  unsigned int pathlen;
  
  if (path == NULL)
    return 0;
  
  if (PATH_SEP == '\\') {
    if ((pathlen = strlen(path)) > 2) {
      if (*(path + 1) == ':'
          && (*(path + 2) == PATH_SEP || *(path + 2) == '/')) /* c:\ or c:/ */
        return 1;
      else if (pathlen > 3 && *path == PATH_SEP && *(path + 1) == PATH_SEP
          && *(path + 2) == '.' /* \\.\ or \\./ */
          && (*(path + 3) == PATH_SEP || *(path + 3) == '/'))
        return 1;
      return 0;
    }
    return *path == PATH_SEP;
  }
  return *path == PATH_SEP;
}

/* join a and b into (optional) c */
char *joinpaths(char *c, const char *a, const char *b) {
  unsigned int alen, blen, clen;
  
  if (c == NULL && (c = (char *) malloc(clen + 1)) == NULL)
    return NULL;
  alen = (a != NULL ? strlen(a) : 0);
  blen = (b != NULL ? strlen(b) : 0);
  clen = (isabspath(b) == 0 /* don't bother calculating if b is absolute */
    ? alen + blen + (alen && *(a + alen - 1) != PATH_SEP ? 1 : 0) : blen);
  
  if (!isabspath(b)) {
    if (alen)
      strcpy(c, a);
    
    if (blen) {
      if (alen && *(a + alen - 1) != PATH_SEP) {
        *(c + alen) = PATH_SEP;
        strcpy(c + alen + 1, b);
      } else {
        strcpy(c + alen, b);
      }
    }
  } else {
    strcpy(c, b);
  }
  return c;
}

/* normalize a path into (optional) dest */
char *normpath(char *dest, const char *src) {
  unsigned int d, i, malloced, nsegs, srclen, start;
  
  malloced = dest == NULL;
  
  if (src == NULL
      || (dest == NULL && (dest = (char *) malloc(srclen + 1)) == NULL))
    return NULL;
  nsegs = 1;
  srclen = strlen(src);
  
  
  for (i = 0; i < srclen; i++) {
    if (*(src + i) == PATH_SEP)
      nsegs++;
  }
  char *segs[nsegs], *temp;
  
  i = nsegs = 0;
  
  while (i < srclen) { /* split around PATH_SEP */
    start = i;
    
    while (i < srclen && *(src + i) != PATH_SEP)
      i++;
    
    if ((segs[nsegs] = (char *) malloc(i - start + 1)) != NULL) {
      strncpy(segs[nsegs], src + start, i - start);
      *(segs[nsegs] + i - start) = '\0';
    } else {
      segs[nsegs] = NULL;
    }
    i++;
    nsegs++; /* used as segment index */
  }
  i = 0;
  
  while (i < nsegs) { /* normalize */
    if (segs[i] != NULL) {
      if (!strcmp(segs[i], "")) {
        if (i < 2) {
          if (PATH_SEP == '\\') {
            if (!isabspath(src)) {
              free(segs[i]);
              segs[i] = NULL;
            }
          } else if (i) {
            free(segs[i]);
            segs[i] = NULL;
          }
        } else {
          free(segs[i]);
          segs[i] = NULL;
        }
      } else if (!strcmp(segs[i], ".")) {
        free(segs[i]);
        segs[i] = NULL;
      } else if (!strcmp(segs[i], "..")) {
        if (i) {
          free(segs[i - 1]);
          segs[i - 1] = NULL;
        }
        free(segs[i]);
        segs[i] = NULL;
      }
    }
    i++;
  }
  
  for (d = i = 0; i < nsegs; i++) { /* concatenate */
    if (segs[i] != NULL) {
      strcpy(dest + d, segs[i]);
      d += strlen(segs[i]);
      
      if ((!i && isabspath(src) && *src == PATH_SEP)
          || (d && *(dest + d - 1) != PATH_SEP)) {
        *(dest + d) = PATH_SEP;
        d++;
      }
      free(segs[i]);
      segs[i] = NULL;
    }
  }
  
  if (PATH_SEP == '/' && isabspath(src)) { /* preserve absolute path */
    if (d) {
      *(dest + d) = PATH_SEP;
      d++;
    }
  } else if (d
      && *(dest + d - 1) == PATH_SEP) { /* remove trailing separator */
    d--;
  }
  *(dest + d) = '\0';
  temp = dest;
  
  if (malloced
      && (dest = realloc(dest, strlen(dest) + 1)) == NULL) /* reallocate */
    dest = temp;
  return dest;
}
