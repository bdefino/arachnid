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
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansiescape.h"
#include "arachnid.h"
#include "path.h"
#include "specify.h"

/* arachnid search program */

#undef help
#define help() puts("OPTIONS\n" \
  "\t-b, --buflen=INT\tuse a custom buffer size (in bytes)\n" \
  "\t-c, --nocolor\tdon't colorize output\n" \
  "\t-C, --nocount\tdon't show match count\n" \
  "\t-e, --extonly\tsearch path extensions only\n" \
  "\t-i, --ignorecase\tignore case\n" \
  "\t-h, --help\tdisplay this text and exit\n" \
  "\t-l, --followlinks\tfollow all symbolic links\n" \
  "\t-n, --nameonly\tsearch final components of path names only\n" \
  "\t-p, --pathonly\tsearch full path names only\n" \
  "\t-s, --showspecial\tinclude all special devices\n" \
  "\t-t, --total\tinclude the total match count\n" \
  "\t-w, --with=STRING\tif searching extensions, file names,\n" \
  "\t\tor paths was specified, search paths that contain\n" \
  "\t\tSTRING in the specified component\n" \
  "PATH\n" \
  "\tproperly-escaped path\n" \
  "\tor \'-\' for STDIN\n" \
  "\tif only the KEY is supplied, the CWD will be used\n" \
  "KEY\n" \
  "\tsearch string")

#undef usage
#define usage() puts("search PATH for KEY\n" \
  "Usage: arachnid [OPTIONS] [PATH] KEY")

struct {
  size_t total;
} ARACHNID_MAIN_DATA = {
  .total = 0
};

struct {
  size_t buflen;
  int flags;
  char *key;
  int nftw_flags;
  char *path;
  int specify_flags;
  int total;
  char *with;
} ARACHNID_MAIN_ARGS = {
  .buflen = DEFAULT_BUFLEN,
  .flags = 0,
  .key = NULL,
  .nftw_flags = FTW_PHYS,
  .path = ".",
  .specify_flags = 0,
  .total = 0,
  .with = NULL
};

enum { /* compatible with arachnid.h */
  ARACHNID_NO_COLOR = 2,
  ARACHNID_NO_COUNT = 4,
  ARACHNID_SEARCH_EXTENSION_ONLY = 8,
  ARACHNID_SEARCH_NAME_ONLY = 16,
  ARACHNID_SEARCH_PATH_ONLY = 32,
  ARACHNID_SEARCH_ONLY = ARACHNID_SEARCH_EXTENSION_ONLY
    | ARACHNID_SEARCH_NAME_ONLY | ARACHNID_SEARCH_PATH_ONLY,
  ARACHNID_WITH = 64
};

const unsigned int ARACHNID_MAIN_NOPENFD = 32;

int ARACHNID_MAIN_EXIT_STATUS = 0;

/* handler for nftw */
int arachnid_main_nftw_handler(const char *fpath, const struct stat *sb,
    int typeflag, struct FTW *ftwbuf) {
  FILE *file;
  struct stat lsb;
  unsigned long matches, with, search_only;
  char *key, *substring;
  
  if (typeflag == FTW_SL) /* symbolic link */
    lstat(fpath, &lsb);
  matches = 0L;
  search_only = ARACHNID_MAIN_ARGS.flags & ARACHNID_SEARCH_ONLY;
  with = (ARACHNID_MAIN_ARGS.flags & ARACHNID_WITH) && search_only;
  key = (with ? ARACHNID_MAIN_ARGS.with : ARACHNID_MAIN_ARGS.key);
  
  if (specify(sb, (typeflag == FTW_SL ? &lsb : sb),
      ARACHNID_MAIN_ARGS.specify_flags)) { /* specified */
    if (ARACHNID_MAIN_ARGS.flags & ARACHNID_SEARCH_PATH_ONLY) {
      matches = sarachnid(fpath, strlen(fpath), key,
        strlen(key), ARACHNID_MAIN_ARGS.flags);
    } else if (ARACHNID_MAIN_ARGS.flags & ARACHNID_SEARCH_NAME_ONLY) {
      substring = basename(fpath);
      
      if (substring != NULL)
        matches = sarachnid(substring, strlen(substring), key, strlen(key),
          ARACHNID_MAIN_ARGS.flags);
    } else if (!(typeflag & FTW_D)
        && (ARACHNID_MAIN_ARGS.flags & ARACHNID_SEARCH_EXTENSION_ONLY)) {
      substring = extension(fpath);
      
      if (substring != NULL)
        matches = sarachnid(substring, strlen(substring), key, strlen(key),
          ARACHNID_MAIN_ARGS.flags);
    }
    key = ARACHNID_MAIN_ARGS.key; /* (revert to) actual key */
    
    if (((with && matches) || !search_only)
        && (!(typeflag & FTW_D) && (file = fopen(fpath, "rb")) != NULL)) {
      matches = farachnid(file, key,
        strlen(key), ARACHNID_MAIN_ARGS.flags,
        ARACHNID_MAIN_ARGS.buflen);
      fclose(file);
    }
    
    if (matches) { /* print */
      if (!(ARACHNID_MAIN_ARGS.flags & ARACHNID_NO_COUNT))
        printf("%u ", matches);
      
      if (ARACHNID_MAIN_ARGS.flags & ARACHNID_NO_COLOR) {
        puts(fpath);
      } else {
        ansi_fprint_path_st(stdout, fpath, sb);
        putchar('\n');
      }
      ARACHNID_MAIN_DATA.total += matches;
    }
  }
  return ARACHNID_MAIN_EXIT_STATUS; /* non-zero when signalled */
}

/* signal handler */
void arachnid_main_signal_handler(int sig);

/* run arachnid */
int main(int argc, char *argv[]) {
  unsigned int i, opt, signals[5];
  struct option longopts[12];
  
  memcpy(longopts,
    (struct option []) {{"buflen", required_argument, NULL, 'b'},
      {"extonly", no_argument, NULL, 'e'},
      {"followlinks", no_argument, NULL, 'l'},
      {"help", no_argument, NULL, 'h'},
      {"ignorecase", no_argument, NULL, 'i'},
      {"nameonly", no_argument, NULL, 'n'},
      {"nocolor", no_argument, NULL, 'c'},
      {"nocount", no_argument, NULL, 'C'},
      {"pathonly", no_argument, NULL, 'p'},
      {"showspecial", no_argument, NULL, 's'},
      {"total", no_argument, NULL, 't'},
      {"with", required_argument, NULL, 'w'},
      {0, 0, 0, 0}},
    sizeof(longopts));
  
  memcpy(signals, (int []) {SIGHUP, SIGINT, SIGTERM, SIGUSR1, SIGUSR2}, 5);
  
  while ((opt = getopt_long(argc, argv, "-b:cCeihlnpstw:", longopts, NULL))
      != -1) {
    switch (opt) {
      case '\1':
        if (ARACHNID_MAIN_ARGS.key != NULL)
          ARACHNID_MAIN_ARGS.path = ARACHNID_MAIN_ARGS.key;
        ARACHNID_MAIN_ARGS.key = optarg;
        break;
      case 'b':
        if (ARACHNID_MAIN_ARGS.buflen = atoi(optarg))
          break;
        puts("Invalid buffer size.");
        usage();
        return 1;
      case 'c':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_NO_COLOR;
        break;
      case 'C':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_NO_COUNT;
        break;
      case 'e':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_SEARCH_EXTENSION_ONLY;
        break;
      case 'i':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_CASE_INSENSITIVE;
        break;
      case 'h':
        usage();
        help();
        return 0;
      case 'l':
        ARACHNID_MAIN_ARGS.nftw_flags &= ~FTW_PHYS;
        ARACHNID_MAIN_ARGS.specify_flags |= SPECIFY_BLK_LINKS
          | SPECIFY_CHR_LINKS | SPECIFY_DIR_LINKS | SPECIFY_FIFO_LINKS
          | SPECIFY_REG_LINKS;
        break;
      case 'n':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_SEARCH_NAME_ONLY;
        ARACHNID_MAIN_ARGS.specify_flags |= SPECIFY_DIRS;
        break;
      case 'p':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_SEARCH_PATH_ONLY;
        ARACHNID_MAIN_ARGS.specify_flags |= SPECIFY_DIRS;
        break;
      case 's':
        ARACHNID_MAIN_ARGS.specify_flags |= SPECIFY_BLKS | SPECIFY_CHRS
          | SPECIFY_FIFOS;
        break;
      case 't':
        ARACHNID_MAIN_ARGS.total = 1;
        break;
      case 'w':
        ARACHNID_MAIN_ARGS.flags |= ARACHNID_WITH;
        ARACHNID_MAIN_ARGS.with = optarg;
        break;
      default:
        usage();
        return 1;
    }
  }
  
  if (ARACHNID_MAIN_ARGS.flags & ARACHNID_WITH) /* can't search directories */
    ARACHNID_MAIN_ARGS.specify_flags &= ~SPECIFY_DIRS;
  
  if (optind < argc && optind < argc - 2) {
    puts("Too many arguments.");
    usage();
    return 1;
  }
  
  if (ARACHNID_MAIN_ARGS.key == NULL) { /* no key was provided */
    usage();
    return 1;
  }
  
  for (i = 0; i < sizeof(signals); i++)
    signal(signals[i], &arachnid_main_signal_handler);
  
  if (ARACHNID_MAIN_ARGS.path != NULL
      && !strcmp(ARACHNID_MAIN_ARGS.path, "-")) {
    unsigned long matches;
    matches = matches = farachnid(stdin, ARACHNID_MAIN_ARGS.key,
      strlen(ARACHNID_MAIN_ARGS.key), ARACHNID_MAIN_ARGS.flags,
      ARACHNID_MAIN_ARGS.buflen);
    
    if (matches) {
      if (!(ARACHNID_MAIN_ARGS.flags & ARACHNID_NO_COUNT))
        printf("%u ", matches);
      
      if (ARACHNID_MAIN_ARGS.flags & ARACHNID_NO_COLOR)
        puts("STDIN");
      else
        ansi_fprint_path(stdout, "STDIN\n");
    }
  } else {
    nftw(ARACHNID_MAIN_ARGS.path, &arachnid_main_nftw_handler,
      ARACHNID_MAIN_NOPENFD, ARACHNID_MAIN_ARGS.nftw_flags);
    
    if (pspecify(ARACHNID_MAIN_ARGS.path, SPECIFY_DIRS)
        && ARACHNID_MAIN_ARGS.total)
      printf("Total: %u\n", ARACHNID_MAIN_DATA.total);
  }
  return ARACHNID_MAIN_EXIT_STATUS;
}

/* signal handler */
void arachnid_main_signal_handler(int sig) {
  ansi_reset_stdio();
  ARACHNID_MAIN_EXIT_STATUS = 1;
}
