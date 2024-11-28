#include <stdio.h>
#include <stdlib.h>

#include "Errors.h"
#include "Color.h"

#define printIfMatch(error_message, status, code, text)  \
    if (status == code)                                  \
    {                                                    \
        snprintf(error_message, MAX_ERROR_MESSAGE_LEN,   \
                 text, code);                            \
        return error_message;                            \
    };

bool isError(Status status)
{
    return (status != OK);
}

char* describeError(Status status)
{
    char* error_message = (char*)calloc(MAX_ERROR_MESSAGE_LEN, sizeof(char));
    if (error_message == nullptr)
    {
        fprintf(stderr, "Could not allocate memory for error message \n");
        return NULL;
    }

    printIfMatch(error_message, status, OK,
                                            "Err %d: Function DescribeError() called with OK status. \n");
    printIfMatch(error_message, status, HELP_FLAG,
                                            "Err %d: Flag \"--help\" was inputted, program ended. \n");
    printIfMatch(error_message, status, UNKNOWN_ARG_ERR,
                                            "Err %d: Error, found unknown command. \n");
    printIfMatch(error_message, status, FILE_OPEN_ERR,
                                            "Err %d: Error, cannot find/open file. \n");
    printIfMatch(error_message, status, ARG_DUPLICATION_ERR,
                                            "Err %d: Error, command duplication is forbidden. \n");
    printIfMatch(error_message, status, MEMORY_ALLOC_ERR,
                                            "Err %d: Error, cannot allocate memory. \n");
    printIfMatch(error_message, status, STREAM_REUSE_ERR,
                                            "Err %d: Error, stream was already set. \n");
    printIfMatch(error_message, status, CAPACITY_LIMIT_ERR,
                                            "Err %d: Error, stack capacity exceeded limit. \n");
    printIfMatch(error_message, status, STACK_OVERFLOW_ERR,
                                            "Err %d: Error, stack capacity is less then its size. \n");
    printIfMatch(error_message, status, POISON_VALUE_ERR,
                                            "Err %d: Error, stack size or capacity is less then allowed. \n");
    printIfMatch(error_message, status, DUMP_FILE_ERR,
                                            "Err %d: Error, couldn`t create dump file. \n");
    printIfMatch(error_message, status, DUMPED,
                                            "Err %d: Stack was dumped, program ended. \n");
    printIfMatch(error_message, status, FILE_NAME_LEN_ERR,
                                            "Err %d: Error, file name is too long (max 16 letters) or fail during "
                                            "creating file path. \n");
    printIfMatch(error_message, status, STACK_UNDERFLOW_ERR,
                                            "Err %d: Error, stack size is less than zero. \n");
    printIfMatch(error_message, status, STACK_DATA_DAMAGED,
                                            "Err %d: Error, stack data was damaged. \n");
    printIfMatch(error_message, status, STACK_FIELD_DAMAGED,
                                            "Err %d: Error, stack fields were damaged. \n");
    printIfMatch(error_message, status, NO_PRINT_FUNC_ERR,
                                            "Err %d: Error, stack must have at least print function. \n");
    if (error_message == nullptr)
    {
        fprintf(stderr, "Unknown error \n");
    }
    return NULL;
}
