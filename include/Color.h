#ifndef COLOR_H
#define COLOR_H

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

#define RED_(str)     (_RED     str _RESET)
#define GREEN_(str)   (_GREEN   str _RESET)
#define YELLOW_(str)  (_YELLOW  str _RESET)
#define BLUE_(str)    (_BLUE    str _RESET)
#define MAGENTA_(str) (_MAGENTA str _RESET)
#define CYAN_(str)    (_CYAN    str _RESET)

void colorPrint(const char *color, const char *format, ...);

#endif //COLOR_H
