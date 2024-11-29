#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <stdlib.h>

const size_t MAX_ERROR_MESSAGE_LEN = 128;

#define checkStatus(status)                                  \
    if (isError(status))                                     \
    {                                                        \
        printErrorMessage(stderr, status);                   \
        return status;                                       \
    };

#define returnIfError(status)   \
    if (isError(status))        \
    {                           \
        return status;          \
    };

enum Status
{
    OK                   = 0,
    HELP_FLAG            = 1,
    UNKNOWN_ARG_ERR      = 2,
    FILE_OPEN_ERR        = 3,
    ARG_DUPLICATION_ERR  = 4,
    MEMORY_ALLOC_ERR     = 5,
    STREAM_REUSE_ERR     = 6,
    CAPACITY_VALUE_ERR   = 7,
    STACK_OVERFLOW_ERR   = 8,
    POISON_VALUE_ERR     = 9,
    DUMP_FILE_ERR        = 10,
    DUMPED               = 11,
    FILE_NAME_LEN_ERR    = 12,
    STACK_UNDERFLOW_ERR  = 13,
    STACK_DATA_DAMAGED   = 14,
    STACK_FIELD_DAMAGED  = 15,
    NO_PRINT_FUNC_ERR    = 16,
    SIM_NUM_OF_ARGS_ERR  = 17,
    SIM_UNKNOWN_ARG_ERR  = 18,
    SIM_NO_ARGS_ERR      = 19,
    SIM_STACK_TYPE_ERR   = 20,
    SIM_NO_TYPE_ERR      = 21
};

bool isError(Status status);
void printErrorMessage(FILE* stream, Status status);

#endif //ERRORCODES_H
