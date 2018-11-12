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

#include "ansiescape.h"

/* ANSI terminal escape codes */

const unsigned int ANSI_RESET_MAP[] = { /* code -> reset code */
  0, 22, 22, 23, 24, 25, 25, 27, 28, 29, 10, 10, 10, 10, 10, 10,
  10, 10, 10, 10, 23, 24, 0, 0, 0, 0, 0, 0, 0, 0, 39, 39,
  39, 39, 39, 39, 39, 39, 39, 0, 49, 49, 49, 49, 49, 49, 49, 49,
  49, 0, 0, 54, 54, 55, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65,
  65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* safely print a string wrapped in the specified codes to a FILE * */
void ansi_fnprint(FILE *fp, const char *src, unsigned int n,
    unsigned int ncodes, unsigned int codes[]) {
  unsigned int i;
  
  if (fp != NULL && src != NULL) {
    if (ncodes) {
      fprintf(fp, "\x1b[");
      
      for (i = 0; i < ncodes - 1; i++)
        fprintf(fp, "%u;", codes[i] % 256);
      fprintf(fp, "%um", codes[i] % 256);
    }
    
    for (i = 0; i < n; i++)
      fputc(*(src + i), fp);
    
    if (ncodes) {
      fprintf(fp, "\x1b[");
      
      for (i = 0; i < ncodes - 1; i++)
        fprintf(fp, "%u;", ANSI_RESET_MAP[codes[i] % 256]);
      fprintf(fp, "%um", ANSI_RESET_MAP[codes[i] % 256]);
    }
    fflush(fp);
  }
}

/* safely print a string wrapped in the specified codes to a FILE * */
void ansi_fprint(FILE *fp, const char *src, unsigned int ncodes,
    unsigned int codes[]) {
  ansi_fnprint(fp, src, strlen(src), ncodes, codes);
}
/* safely print a path based on its mode to a FILE * */
void ansi_fprint_path(FILE *fp, const char *path) {
  struct stat st;
  
  memset(&st, '\0', sizeof(struct stat));
  stat(path, &st);
  
  ansi_fprint_path_st(fp, path, &st);
}

/* safely print a path based on its mode to a FILE * */
void ansi_fprint_path_st(FILE *fp, const char *path, const struct stat *st) {
  unsigned int codes[2], ncodes;
  
  if (fp != NULL && path != NULL && st != NULL) {
    codes[0] = ANSI_FG_BRIGHT_MAGENTA; /* unknown mode */
    codes[1] = ANSI_BOLD;
    ncodes = 2;
    
    if (S_ISREG(st->st_mode)) {
      codes[0] = ANSI_FG_ORIGINAL;
      ncodes = 1;
    } else if (S_ISDIR(st->st_mode)) {
      codes[0] = ANSI_FG_BRIGHT_BLUE;
    } else if (S_ISBLK(st->st_mode)) {
      codes[0] = ANSI_FG_BRIGHT_CYAN;
    } else if (S_ISCHR(st->st_mode)) {
      codes[0] = ANSI_FG_BRIGHT_GREEN;
    } else if (S_ISFIFO(st->st_mode)) {
      codes[0] = ANSI_FG_BRIGHT_YELLOW;
    }
    ansi_fprint(fp, path, ncodes, codes);
  }
}

/* reset STDIO */
void ansi_reset_stdio(void) {
  ANSI_FRESET(stderr);
  ANSI_FRESET(stdin);
  ANSI_FRESET(stdout);
}
