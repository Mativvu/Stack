#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <string.h>

#include "Stack.h"
#include "Errors.h"

#define unused(param) while(0){ (void)(param); };

const size_t MAX_LINE_LEN = 64;
const size_t MAX_TERM_LEN = 32;
const size_t MAX_ARGS_NUM = 8;

const char SEPARATOR = ' ';

typedef Status (*stack_func)(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);

typedef struct StackCommand
{
    const char* name;
    stack_func  function;
} StackCommand;

Status stackSimulation(FILE* stream_in, FILE* stream_out, Stack* stack);

Status parseStackCommands(char**        args_array,     size_t args_array_size,
                          StackCommand* commands_array, size_t commands_array_size, Stack* stack, FILE* stream_out);

char* getNextArgument(char** args_array, size_t args_array_size, size_t* arg_index);

Status linkStringPointers(char* string, size_t num_of_args, char** arguments_array);
Status myStringReplace   (char* string, char old_symbol, char new_symbol, size_t* count);
Status myStringLen       (char* string, size_t* size);

Status setStackType   (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulatePush   (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulatePop    (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulatePeek   (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateDump   (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateVerify (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateDestroy(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateFit    (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateClear  (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateGetSize(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateGetCap (Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);

#endif //SIMULATION_H

