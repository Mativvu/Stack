#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "StackSim.h"
#include "Errors.h"
#include "Stack.h"
#include "Debug.h"

static char stack_data_type = 0;

static StackCommand stack_commands_array[] =
{
    {"type",    setStackType   },
    {"push",    simulatePush   },
    {"pop",     simulatePop    },
    {"peek",    simulatePeek   },
    {"dump",    simulateDump   },
    {"verify",  simulateVerify },
    {"destroy", simulateDestroy},
    {"fit",     simulateFit    },
    {"clear",   simulateClear  },
    {"size",    simulateGetSize},
    {"cap",     simulateGetCap }
};
static const size_t stack_commands_array_size = sizeof(stack_commands_array)/sizeof(stack_commands_array[0]);

Status stackSimulation(FILE* stream_in, FILE* stream_out, Stack* stack)
{
    myAssert(stream_in  != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(stack      != nullptr);

    Status status = OK;

    char  buffer[MAX_LINE_LEN]          = {};
    char* arguments_array[MAX_ARGS_NUM] = {};

    while (fgets(buffer, MAX_LINE_LEN - 1, stream_in) != NULL)
    {
        colorPrint(YELLOW, "Got line %s \n", buffer);

        size_t num_of_arguments = 1;
        myStringReplace(buffer, SEPARATOR, '\0', &num_of_arguments);

        myStringReplace(buffer, '\n', '\0', NULL);

        colorPrint(YELLOW, "Line after replacing %s \n", buffer);
        colorPrint(YELLOW, "Line got %zu arguments \n", num_of_arguments);

        if (num_of_arguments > MAX_ARGS_NUM)
        {
            return SIM_NUM_OF_ARGS_ERR;
        }

        linkStringPointers(buffer, num_of_arguments, arguments_array);

        for (size_t index = 0; index < num_of_arguments; ++index)
        {
            colorPrint(YELLOW, "Parsed arg %s \n", arguments_array[index]);
        }

        status = parseStackCommands(arguments_array,      num_of_arguments,
                                    stack_commands_array, stack_commands_array_size, stack, stream_out);
        returnIfError(status);
    }
    return OK;
}

Status parseStackCommands(char**        args_array,     size_t args_array_size,
                          StackCommand* commands_array, size_t commands_array_size, Stack* stack, FILE* stream_out)
{
    myAssert(args_array     != nullptr);
    myAssert(commands_array != nullptr);
    myAssert(stack          != nullptr);
    myAssert(stream_out     != nullptr);

    Status status = OK;

    for (size_t arg_index = 0; arg_index < args_array_size; ++arg_index)
    {
        bool found = false;
        for (size_t cmd_index = 0; cmd_index < commands_array_size; ++cmd_index)
        {
            if (strcmp(args_array[arg_index], commands_array[cmd_index].name) == 0)
            {
                status = commands_array[cmd_index].function(stack, args_array, args_array_size, &arg_index, stream_out);
                returnIfError(status);

                found = true;
                break;
            }
        }
        if (found == false)
        {
            return SIM_UNKNOWN_ARG_ERR;
        }
    }
    return OK;
}

char* getNextArgument(char** args_array, size_t args_array_size, size_t* arg_index)
{
    myAssert(args_array != nullptr);
    myAssert(arg_index  != nullptr);

    if (*arg_index + 1 < args_array_size)
    {
        (*arg_index)++;

        return args_array[*arg_index];
    }
    return nullptr;
}

Status linkStringPointers(char* string, size_t num_of_args, char** arguments_array)
{
    myAssert(string           != nullptr);
    myAssert(arguments_array  != nullptr);

    size_t arg_index      = 0;
    size_t current_length = 0;
    for (size_t string_index = 0; arg_index < num_of_args; ++string_index)
    {
        if (string[string_index] == '\0')
        {
            if (current_length != 0)
            {
                arguments_array[arg_index] = (string + (string_index - current_length));
                arg_index++;
                current_length = 0;
            }
        }
        else
        {
            current_length++;
        }
    }
    return OK;
}

Status myStringReplace(char* string, char old_symbol, char new_symbol, size_t* count)
{
    myAssert(string != nullptr);

    for (size_t index = 0; string[index] != '\0'; ++index)
    {
        if (string[index] == old_symbol)
        {
            string[index] = new_symbol;

            if (count != nullptr)
            {
                (*count)++;
            }
        }
    }
    return OK;
}

Status myStringLen(char* string, size_t* length)
{
    myAssert(string != nullptr);
    myAssert(length != nullptr);

    size_t offset = 0;

    while (string[offset] != '\0' && string[offset] != '\n')
    {
        offset++;
        (*length)++;
    }
    return OK;
}

Status setStackType(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(stack);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    if ((*arg_index) + 1 >= args_array_size)
    {
        return SIM_NO_ARGS_ERR;
    }

    char* type_identifier = getNextArgument(args_array, args_array_size, arg_index);

    size_t type_len = 0;
    myStringLen(type_identifier, &type_len);

    colorPrint(YELLOW, "Parsed type %c of len %i \n", *type_identifier, type_len);

    if (type_identifier == nullptr || type_len > 1)
    {
        return SIM_STACK_TYPE_ERR;
    }

    stack_data_type = *type_identifier;

    fprintf(stream_out, "Set stack data type %c \n", stack_data_type);

    return OK;
}

Status simulatePush(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    if ((*arg_index) + 1 >= args_array_size)
    {
        return SIM_NO_ARGS_ERR;
    }

    char* string_value = getNextArgument(args_array, args_array_size, arg_index);

    if (!stack_data_type)
    {
        return SIM_NO_TYPE_ERR;
    }

    switch (stack_data_type)
    {
        case 'i':
        {
            const void* value = (const void*)((int)strtol(string_value, NULL, 10));

            status = stackPush(stack, value);
            returnIfError(status);

            fprintf(stream_out, "Called stack push \n");
            fprintf(stream_out, "Pushed %d \n", (int)((uintptr_t)(value)));

            break;
        }
        case 'c':
        {
            const void* value = (const void*)((char)string_value[0]);

            status = stackPush(stack, value);
            returnIfError(status);

            fprintf(stream_out, "Called stack push \n");
            fprintf(stream_out, "Pushed %c \n", (char)((uintptr_t)(value)));

            break;
        }
        case 's':
        {
            void* value = (void*)(string_value);

            status = stackPush(stack, value);
            returnIfError(status);

            fprintf(stream_out, "Called stack push \n");
            fprintf(stream_out, "Pushed %s \n", (char*)(value));

            break;
        }
        case 'l':
        {
            void* value = (void*)(strtol(string_value, NULL, 10));

            status = stackPush(stack, value);
            returnIfError(status);

            fprintf(stream_out, "Called stack push \n");
            fprintf(stream_out, "Pushed %ld \n", (long)((uintptr_t)(value)));

            break;
        }
        default:
        {
            return SIM_STACK_TYPE_ERR;
        }
    }
    return status;
}

Status simulatePop(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    if (!stack_data_type)
    {
        return SIM_NO_TYPE_ERR;
    }

    switch (stack_data_type)
    {
        case 'i':
        {
            void* variable = NULL;

            status = stackPop(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack pop \n");
            fprintf(stream_out, "Popped %d \n", (int)((uintptr_t)(variable)));

            break;
        }
        case 'c':
        {
            void* variable = NULL;

            status = stackPop(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack pop \n");
            fprintf(stream_out, "Popped %c \n", (char)((uintptr_t)(variable)));

            break;
        }
        case 's':
        {
            void* variable = NULL;

            status = stackPop(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack pop \n");
            fprintf(stream_out, "Popped %s \n", (char*)(variable));

            break;
        }
        case 'l':
        {
            void* variable = NULL;

            status = stackPop(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack pop \n");
            fprintf(stream_out, "Popped %ld \n", (long)((uintptr_t)(variable)));

            break;
        }
        default:
        {
            return SIM_STACK_TYPE_ERR;
        }
    }
    return status;
}

Status simulatePeek(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    if (!stack_data_type)
    {
        return SIM_NO_TYPE_ERR;
    }

    switch (stack_data_type)
    {
        case 'i':
        {
            void* variable = NULL;

            status = stackPeek(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack peek \n");
            fprintf(stream_out, "Peeked %d \n", (int)((uintptr_t)(variable)));

            break;
        }
        case 'c':
        {
            void* variable = NULL;

            status = stackPeek(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack peek \n");
            fprintf(stream_out, "Peeked %c \n", (char)((uintptr_t)(variable)));

            break;
        }
        case 's':
        {
            void* variable = NULL;

            status = stackPeek(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack peek \n");
            fprintf(stream_out, "Peeked %s \n", (char*)(variable));

            break;
        }
        case 'l':
        {
            void* variable = NULL;

            status = stackPeek(stack, &variable);
            returnIfError(status);

            fprintf(stream_out, "Called stack peek \n");
            fprintf(stream_out, "Peeked %ld \n", (long)((uintptr_t)(variable)));

            break;
        }
        default:
        {
            return SIM_STACK_TYPE_ERR;
        }
    }
    return status;
}

Status simulateDump(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackDump(stack, status);
    fprintf(stream_out, "Called stack dump \n");

    return status;
}

Status simulateVerify(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackVerify(stack);
    fprintf(stream_out, "Called stack verify \n");

    return status;
}

Status simulateDestroy(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackDestructor(stack);
    fprintf(stream_out, "Called stack destructor \n");

    return status;
}

Status simulateFit(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackFitToSize(stack);
    fprintf(stream_out, "Called stack fit to size \n");

    return status;
}

Status simulateClear(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackClear(stack);
    fprintf(stream_out, "Called stack clear \n");

    return status;
}

Status simulateGetSize(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    size_t stack_size = stackGetSize(stack);
    fprintf(stream_out, "Called stack get size \n");
    fprintf(stream_out, "Stack size is %zu \n", stack_size);

    return OK;
}

Status simulateGetCap(Stack* stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    size_t stack_capacity = stackGetCapacity(stack);
    fprintf(stream_out, "Called stack get capacity \n");
    fprintf(stream_out, "Stack capacity is %zu \n", stack_capacity);

    return OK;
}
