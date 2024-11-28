#ifndef STACKVOID_H
#define STACKVOID_H

#include "Errors.h"

const char* const DUMP_FILE_PATH = "files/dump.txt";

const size_t POISON_SIZE_T = (size_t)(-1);
const size_t MAX_CAPACITY  = 1e7;
const size_t ZERO          = 0;

typedef void  (*print_func)(FILE* stream, const void* stack_elem);
typedef void* (*clone_func)(const void* value);
typedef void  (*free_func) (void* stack_elem);

typedef unsigned long long ull;
const ull CANARY_LEFT  = 0x0000000000BAD1ABA;
const ull CANARY_RIGHT = 0x000000000005ADDA4;

typedef struct myStack Stack;

Status stackConstructor(Stack** stack, size_t capacity, print_func print, clone_func clone, free_func free);
Status stackPop        (Stack* stack, void** variable);
Status stackPush       (Stack* stack, const void* value);
Status stackDump       (Stack* stack, Status error);
Status stackDestructor (Stack* stack);
Status stackVerify     (Stack* stack);
Status stackResize     (Stack* stack, size_t new_size);
Status stackPeek       (Stack* stack, void** variable);
Status stackFitToSize  (Stack* stack);
Status stackClear      (Stack* stack);

size_t stackGetCapacity(Stack* stack);
size_t stackGetSize    (Stack* stack);

void print_str (FILE* stream, const void* stack_elem);
void print_int (FILE* stream, const void* stack_elem);
void print_char(FILE* stream, const void* stack_elem);

void* clone_str(const void* value);

void free_str(void* stack_elem);

#endif //STACKVOID_H
