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
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

/* Pythonic path manipulations *//* deprecate PATH_MAX */

#ifndef PATH_H
#define PATH_H

#undef PATH_SEP
#define PATH_SEP '/'

#ifdef _WIN32
#undef getcwd
#define getcwd(b, s) _getcwd(b, s)

#undef PATH_SEP
#define PATH_SEP '\\'
#endif

/* place an absolute path into (optional) dest */
char *abspath(char *dest, const char *src);

/* return a pointer to the base name */
char *basename(const char *src);

/* place directory (if available) in (optional) dest */
char *dirname(char *dest, const char *src);

/* return a pointer to the extension */
char *extension(const char *src);

/* return whether the path is absolute */
unsigned int isabspath(const char *path);

/* join a and b into (optional) c */
char *joinpaths(char *c, const char *a, const char *b);

/* normalize a path into (optional) dest */
char *normpath(char *dest, const char *src);

#endif
