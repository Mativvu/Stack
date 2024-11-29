#include <stdio.h>
#include <stdlib.h>

#include "Errors.h"
#include "Color.h"

#define printIfMatch(stream, status, code, text)   \
    if (status == code)                            \
    {                                              \
        fprintf(stream, "Err %d: %s", code, text); \
        return;                                    \
    };

bool isError(Status status)
{
    return (status != OK);
}

void printErrorMessage(FILE* stream, Status status)
{
    printIfMatch(stream, status, OK,
                                            "Err %d: Function DescribeError() called with OK status. \n");
    printIfMatch(stream, status, HELP_FLAG,
                                            "Err %d: Flag \"--help\" was inputted, program ended. \n");
    printIfMatch(stream, status, UNKNOWN_ARG_ERR,
                                            "Err %d: Error, found unknown command. \n");
    printIfMatch(stream, status, FILE_OPEN_ERR,
                                            "Err %d: Error, cannot find/open file. \n");
    printIfMatch(stream, status, ARG_DUPLICATION_ERR,
                                            "Err %d: Error, command duplication is forbidden. \n");
    printIfMatch(stream, status, MEMORY_ALLOC_ERR,
                                            "Err %d: Error, cannot allocate memory. \n");
    printIfMatch(stream, status, STREAM_REUSE_ERR,
                                            "Err %d: Error, stream was already set. \n");
    printIfMatch(stream, status, CAPACITY_VALUE_ERR,
                                            "Err %d: Error, stack capacity exceeded limit. \n");
    printIfMatch(stream, status, STACK_OVERFLOW_ERR,
                                            "Err %d: Error, stack capacity is less then its size. \n");
    printIfMatch(stream, status, POISON_VALUE_ERR,
                                            "Err %d: Error, stack size or capacity is less then allowed. \n");
    printIfMatch(stream, status, DUMP_FILE_ERR,
                                            "Err %d: Error, couldn`t create dump file. \n");
    printIfMatch(stream, status, DUMPED,
                                            "Err %d: Stack was dumped, program ended. \n");
    printIfMatch(stream, status, FILE_NAME_LEN_ERR,
                                            "Err %d: Error, file name is too long (max 16 letters) or fail during "
                                            "creating file path. \n");
    printIfMatch(stream, status, STACK_UNDERFLOW_ERR,
                                            "Err %d: Error, stack size is less than zero. \n");
    printIfMatch(stream, status, STACK_DATA_DAMAGED,
                                            "Err %d: Error, stack data was damaged. \n");
    printIfMatch(stream, status, STACK_FIELD_DAMAGED,
                                            "Err %d: Error, stack fields were damaged. \n");
    printIfMatch(stream, status, NO_PRINT_FUNC_ERR,
                                            "Err %d: Error, stack must have at least print function. \n");
    printIfMatch(stream, status, SIM_NUM_OF_ARGS_ERR,
                                            "Err %d: Error(sim), number on arguments in line is too big. \n");
    printIfMatch(stream, status, SIM_UNKNOWN_ARG_ERR,
                                            "Err %d: Error(sim), found unknown command. \n");
    printIfMatch(stream, status, SIM_NO_ARGS_ERR,
                                            "Err %d: Error(sim), not enough arguments for stack command. \n");
    printIfMatch(stream, status, SIM_STACK_TYPE_ERR,
                                            "Err %d: Error(sim), no such stack data type. \n");
    printIfMatch(stream, status, SIM_NO_TYPE_ERR,
                                            "Err %d: Error(sim), stack data type was not set. \n");
    fprintf(stream, "Unknown error \n");
    return;
}
