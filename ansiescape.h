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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* ANSI terminal escape codes */

#ifndef ANSICOLORS_H
#define ANSICOLORS_H

enum { /* ideograms and multiple definitions aren't supported */
  ANSI_RESET = 0, /* resets all attributes */
  
  /* text effects */
  
  ANSI_BOLD = 1,
  ANSI_FAINT = 2,
  ANSI_ITALIC = 3,
  ANSI_UNDERLINE = 4,
  ANSI_SLOW_BLINK = 5,
  ANSI_FAST_BLINK = 6,
  ANSI_SWAP_COLORS = 7,
  ANSI_CONCEAL = 8,
  ANSI_STRIKETHROUGH = 9,
  ANSI_PRIMARY_FONT = 10,
  ANSI_FRAKTUR = 20,
  ANSI_DOUBLE_UNDERLINE = 21,
  
  /* colors */
  
  ANSI_FG_BLACK = 30,
  ANSI_FG_RED = 31,
  ANSI_FG_GREEN = 32,
  ANSI_FG_YELLOW = 33,
  ANSI_FG_BLUE = 34,
  ANSI_FG_MAGENTA = 35,
  ANSI_FG_CYAN = 36,
  ANSI_FG_WHITE = 37,
  
  ANSI_FG_ORIGINAL = 39, /* reverts to original foreground */
  
  ANSI_BG_BLACK = ANSI_FG_BLACK + 10,
  ANSI_BG_RED = ANSI_FG_RED + 10,
  ANSI_BG_GREEN = ANSI_FG_GREEN + 10,
  ANSI_BG_YELLOW = ANSI_FG_YELLOW + 10,
  ANSI_BG_BLUE = ANSI_FG_BLUE + 10,
  ANSI_BG_MAGENTA = ANSI_FG_MAGENTA + 10,
  ANSI_BG_CYAN = ANSI_FG_CYAN + 10,
  ANSI_BG_WHITE = ANSI_FG_WHITE + 10,
  
  ANSI_BG_ORIGINAL = 49, /* reverts to original background */
  
  /* text decorations */
  
  ANSI_FRAMED = 51,
  ANSI_ENCIRCLED = 52,
  ANSI_OVERLINED = 53,
  
  /* bright colors */
  
  ANSI_FG_BRIGHT_BLACK = 90,
  ANSI_FG_BRIGHT_RED = 91,
  ANSI_FG_BRIGHT_GREEN = 92,
  ANSI_FG_BRIGHT_YELLOW = 93,
  ANSI_FG_BRIGHT_BLUE = 94,
  ANSI_FG_BRIGHT_MAGENTA = 95,
  ANSI_FG_BRIGHT_CYAN = 96,
  ANSI_FG_BRIGHT_WHITE = 97,
  
  ANSI_BG_BRIGHT_BLACK = ANSI_FG_BRIGHT_BLACK + 10,
  ANSI_BG_BRIGHT_RED = ANSI_FG_BRIGHT_RED + 10,
  ANSI_BG_BRIGHT_GREEN = ANSI_FG_BRIGHT_GREEN + 10,
  ANSI_BG_BRIGHT_YELLOW = ANSI_FG_BRIGHT_YELLOW + 10,
  ANSI_BG_BRIGHT_BLUE = ANSI_FG_BRIGHT_BLUE + 10,
  ANSI_BG_BRIGHT_MAGENTA = ANSI_FG_BRIGHT_MAGENTA + 10,
  ANSI_BG_BRIGHT_CYAN = ANSI_FG_BRIGHT_CYAN + 10,
  ANSI_BG_BRIGHT_WHITE = ANSI_FG_BRIGHT_WHITE + 10,
  
  /* grayscale boundaries */
  
  ANSI_GRAYSCALE_BLACK = 232,
  ANSI_GRAYSCALE_WHITE = 255
};

extern const unsigned int ANSI_RESET_MAP[256]; /* code -> reset code */

#ifndef _WIN32 /* POSIX */

/* print the reset code to a FILE * */
#undef ANSI_FRESET
#define ANSI_FRESET(fp) fprintf((FILE *) (fp), "\x1b[0m")

/* print the code's reset code to a FILE * */
#undef ANSI_FRESET_CODE
#define ANSI_FRESET_CODE(fp, c) ( \
  (FILE *) (fp) != NULL \
    ? fprintf((FILE *) (fp), "\x1b[%dm", ANSI_RESET_MAP[((int) (c)) % 256]) \
    : 0)

/* print the code to a FILE * */
#undef ANSI_FSET
#define ANSI_FSET(fp, c) ((FILE *) (fp) != NULL \
  ? fprintf((FILE *) (fp), "\x1b[%dm", ((int) (c)) % 256) : 0)

#else /* NT terminals generally don't accept ANSI escape codes */

/* do nothing */
#undef ANSI_FRESET
#define ANSI_FRESET(fp)

/* do nothing */
#undef ANSI_FRESET_CODE
#define ANSI_FRESET_CODE(fp, c)

/* do nothing */
#undef ANSI_FSET
#define ANSI_FSET(fp)

#endif

/* safely print a string wrapped in the specified codes to a FILE * */
void ansi_fnprint(FILE *fp, const char *src, unsigned int n,
  unsigned int ncodes, unsigned int codes[]);

/* safely print a string wrapped in the specified codes to a FILE * */
void ansi_fprint(FILE *fp, const char *src, unsigned int ncodes,
  unsigned int codes[]);

/* safely print a path based on its mode to a FILE * */
void ansi_fprint_path(FILE *fp, const char *path);

/* safely print a path based on its mode to a FILE * */
void ansi_fprint_path_st(FILE *fp, const char *path, const struct stat *st);

/* reset STDIO */
void ansi_reset_stdio(void);

#endif
