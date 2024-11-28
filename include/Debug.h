#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#include "Color.h"

#define DEBUG

#ifdef DEBUG

#define debugPrintString(str)         \
    printf(_GREEN "%s" _RESET, str);

#define myAssert(expr)                                      \
    if(!(expr))                                             \
    {                                                       \
        printf(_BLUE "Assert fail expr: " #expr "\n"        \
               "line %d \nfunction: %s \nfile %s\n" _RESET, \
               __LINE__, __PRETTY_FUNCTION__, __FILE__);    \
        abort();                                            \
    };

#else
#define debugPrintString(str) ;
#define myAssert(expr) ;

#endif //DEBUG

#endif //DEBUG_H
