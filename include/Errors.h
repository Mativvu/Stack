#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <stdlib.h>

const size_t MAX_ERROR_MESSAGE_LEN = 128;

#define checkStatus(status)                                  \
    if (isError(status))                                     \
    {                                                        \
        char* error_message = describeError(status);         \
        fprintf(stderr, _RED "%s" _RESET, error_message);    \
        free(error_message);                                 \
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
    CAPACITY_LIMIT_ERR   = 7,
    STACK_OVERFLOW_ERR   = 8,
    POISON_VALUE_ERR     = 9,
    DUMP_FILE_ERR        = 10,
    DUMPED               = 11,
    FILE_NAME_LEN_ERR    = 12,
    STACK_UNDERFLOW_ERR  = 13,
    STACK_DATA_DAMAGED   = 14,
    STACK_FIELD_DAMAGED  = 15,
    NO_PRINT_FUNC_ERR    = 16
};

bool isError(Status status);
char* describeError(Status status);

#endif //ERRORCODES_H
