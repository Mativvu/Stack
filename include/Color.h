#ifndef COLOR_H
#define COLOR_H

#define __RED(str) _RED str _RESET

const char* const RED    = "\x1b[31m";
const char* const GREEN  = "\x1b[32m";
const char* const YELLOW = "\x1b[33m";
const char* const BLUE   = "\x1b[34m";
const char* const MAGENTA= "\x1b[35m";
const char* const CYAN   = "\x1b[36m";
const char* const RESET  = "\x1b[0m";

#define _RED     "\x1b[31m"
#define _GREEN   "\x1b[32m"
#define _YELLOW  "\x1b[33m"
#define _BLUE    "\x1b[34m"
#define _MAGENTA "\x1b[35m"
#define _CYAN    "\x1b[36m"
#define _RESET   "\x1b[0m"

void colorPrint(const char *color, const char *format, ...);

#endif //COLOR_H
