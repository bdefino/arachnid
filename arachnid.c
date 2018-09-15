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

#include "arachnid.h"

/* arachnid search utility */

/* return the number of times a key appears in a file */
unsigned long farachnid(FILE *fp, const char *key, size_t key_size,
    int flags, size_t buflen) {
  char buffer[buflen], c, cskey[key_size];
  unsigned int bytes_read, chars_matched, i, size, start;
  unsigned long matches;
  
  matches = 0L;
  
  if (fp != NULL && key != NULL) {
    start = ftell(fp);
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp) - start;
    fseek(fp, start, SEEK_SET);
    
    if (key_size == 0) /* empty string */
      return size + 1L;
    buflen = (!buflen ? DEFAULT_BUFLEN : buflen);
    
    for (i = 0; i < key_size; i++)
      *(cskey + i) = (flags & ARACHNID_CASE_INSENSITIVE
        ? tolower(*(key + i)) : *(key + i));
    chars_matched = 0;
    
    while (bytes_read = fread(buffer, 1, buflen, fp)) { /* search */
      for (i = 0; i < bytes_read; i++) {
        c = *(buffer + i);
        c = (flags & ARACHNID_CASE_INSENSITIVE ? tolower(c) : c);
        
        if (*(cskey + chars_matched) == c) {
          chars_matched++;
          
          if (chars_matched == key_size) {
            chars_matched = 0;
            matches++;
          }
        } else {
          chars_matched = 0;
        }
      }
    }
  }
  return matches;
}

/* return the number of times a key appears at a path */
unsigned long parachnid(const char *path, const char *key,
    size_t key_size, int flags, size_t buflen) {
  FILE *fp;
  unsigned long matches;
  struct stat path_stat;
  
  matches = 0L;
  
  if (!stat(path, &path_stat) && !S_ISDIR(path_stat.st_mode)
      && (fp = fopen(path, "rb")) != NULL) {
    matches = farachnid(fp, key, key_size, flags, buflen);
    fclose(fp);
  }
  return matches;
}

/* return the number of times a key appears in a string */
unsigned long sarachnid(const char *string, size_t size, const char *key,
    size_t key_size, int flags) {
  char c, cskey[key_size];
  unsigned int chars_matched, i;
  unsigned long matches;
  
  matches = 0L;
  
  if (key_size == 0) /* empty string */
    return strlen(string) + 1L;
  
  if (key != NULL && string != NULL && key_size <= size) {
    for (i = 0; i < key_size; i++)
      *(cskey + i) = (flags & ARACHNID_CASE_INSENSITIVE
        ? tolower(*(key + i)) : *(key + i));
    chars_matched = 0;
    
    for (i = 0; i < size; i++) { /* search */
      c = *(string + i);
      c = (flags & ARACHNID_CASE_INSENSITIVE ? tolower(c) : c);
      
      if (*(cskey + chars_matched) == c) {
        chars_matched++;
        
        if (chars_matched == key_size) {
          chars_matched = 0;
          matches++;
        }
      } else {
        chars_matched = 0;
      }
    }
  }
  return matches;
}
