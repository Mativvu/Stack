#ifndef STACKNORM_H
#define STACKNORM_H

#include "Errors.h"

const char* const DUMP_FILE_PATH = "files/dump.txt";

const size_t POISON_SIZE_T = (size_t)(-1);
const size_t MAX_CAPACITY  = 1e7;
const size_t ZERO          = 0;

typedef double elem_t;
#define ELEM_T "lg"

typedef unsigned long long ull;
const ull CANARY_LEFT  = 0x0000000000BAD1ABA;
const ull CANARY_RIGHT = 0x000000000005ADDA4;
const size_t  ELEMS_PER_CANARY = (size_t)(sizeof(ull)/sizeof(elem_t));

typedef struct myStack Stack;

Status stackConstructor(Stack** stack, size_t capacity);
Status stackPop        (Stack* stack, elem_t* variable);
Status stackPush       (Stack* stack, const elem_t value);
Status stackDump       (Stack* stack, Status error);
Status stackDestructor (Stack* stack);
Status stackVerify     (Stack* stack);
Status stackResize     (Stack* stack, size_t new_size);

size_t stackGetCapacity(Stack* stack);
size_t stackGetSize    (Stack* stack);

#endif //STACKNORM_H
