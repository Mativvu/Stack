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
    FILE_NAME_LEN_ERR    = 11,
    STACK_UNDERFLOW_ERR  = 12,
    STACK_DATA_DAMAGED   = 13,
    STACK_FIELD_DAMAGED  = 14,
    NO_PRINT_FUNC_ERR    = 15,
    SIM_NUM_OF_ARGS_ERR  = 16,
    SIM_UNKNOWN_ARG_ERR  = 17,
    SIM_NO_ARGS_ERR      = 18,
    SIM_STACK_TYPE_ERR   = 19,
    SIM_NO_TYPE_ERR      = 20,
    FILE_READ_ERR        = 21,
    FILE_SIZE_ERR        = 22,
    FILE_SEEK_ERR        = 23,
    SIM_UNKNOWN_TYPE_ERR = 24,
    SIM_WRONG_TYPE_ERR   = 25
};

bool isError(Status status);
void printErrorMessage(FILE* stream, Status status);

#endif //ERRORCODES_H
