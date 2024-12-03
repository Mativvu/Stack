#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <string.h>

#include "Stack.h"
#include "Errors.h"

#define destroyIfError(status, stack) \
    {                                 \
        if (isError(status))          \
        {                             \
            stackDestructor(*stack);  \
        }                             \
    }

#define unused(param) while(0){ (void)(param); };

const size_t MAX_FILE_SIZE = 8192;
const size_t MAX_LINE_NUM  = 256;
const size_t MAX_LINE_LEN  = 512;
const size_t MAX_TERM_LEN  = 32;
const size_t MAX_ARGS_NUM  = 128;

const char SEPARATOR = ' ';

typedef Status (*stack_func)(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);

typedef Status (*stackCtorType)(Stack** stack);
typedef Status (*stackPushType)(Stack*  stack,  const char* string    );
typedef Status (*stackPeekVar) (Stack*  stack,  void**      variable  );
typedef Status (*stackPopVar)  (Stack*  stack,  void**      variable  );

typedef struct StackCommand
{
    const char* name;
    stack_func  function;
} StackCommand;

typedef struct StackTypeFunction
{
    const char    type_id;
    stackCtorType ctor_func;
    stackPushType push_func;
    stackPeekVar  peek_func;
    stackPopVar   pop_func;
} StackTypeFunction;

Status stackSimulation     (FILE* stream_in, FILE* stream_out, Stack** stack);
Status simulateFromTerminal(FILE* stream_in, FILE* stream_out, Stack** stack);
Status simulateFromFile    (FILE* stream_in, FILE* stream_out, Stack** stack);

Status parseStackCommands(char**        args_array,     size_t args_array_size,
                          StackCommand* commands_array, size_t commands_array_size, Stack** stack, FILE* stream_out);

char* getNextArgument(char** args_array, size_t args_array_size, size_t* arg_index);

Status readDataFromFile(FILE* stream_in, char*   array,    size_t* array_size);
Status getFileSize     (FILE* stream_in, size_t* file_size);

Status linkStringPointers(char* string, size_t  num_of_args, char**  arguments_array);
Status myStringReplace   (char* string, char    old_symbol,  char    new_symbol     );
Status myStringCount     (char* string, char    symbol,      size_t* count          );
Status myStringLen       (char* string, size_t* size);

Status setStackType   (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateCtor   (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulatePush   (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulatePop    (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulatePeek   (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateDump   (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateVerify (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateDtor   (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateFit    (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateClear  (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateGetSize(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);
Status simulateGetCap (Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out);

Status ctorInt  (Stack** stack);
Status ctorChar (Stack** stack);
Status ctorLong (Stack** stack);
Status ctorStr  (Stack** stack);

Status pushInt  (Stack* stack, const char* string);
Status pushChar (Stack* stack, const char* string);
Status pushLong (Stack* stack, const char* string);
Status pushStr  (Stack* stack, const char* string);

Status peekVar  (Stack* stack, void** variable);
Status popVar   (Stack* stack, void** variable);

#endif //SIMULATION_H

