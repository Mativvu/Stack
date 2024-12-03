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
    {"create",  simulateCtor   },
    {"push",    simulatePush   },
    {"pop",     simulatePop    },
    {"peek",    simulatePeek   },
    {"dump",    simulateDump   },
    {"verify",  simulateVerify },
    {"destroy", simulateDtor   },
    {"fit",     simulateFit    },
    {"clear",   simulateClear  },
    {"size",    simulateGetSize},
    {"cap",     simulateGetCap }
};
static const size_t stack_commands_array_size = sizeof(stack_commands_array)/sizeof(stack_commands_array[0]);

static StackTypeFunction stack_type_functions_array[] =
{
    {'i', ctorInt,  pushInt,  peekVar, popVar},
    {'c', ctorChar, pushChar, peekVar, popVar},
    {'l', ctorLong, pushLong, peekVar, popVar},
    {'s', ctorStr,  pushStr,  peekVar, popVar}
};
static const size_t stack_type_functions_array_size = sizeof(stack_type_functions_array)/sizeof(stack_type_functions_array[0]);

Status stackSimulation(FILE* stream_in, FILE* stream_out, Stack** stack)
{
    myAssert(stack      != nullptr);
    myAssert(stream_in  != nullptr);
    myAssert(stream_out != nullptr);

    if (stream_in == stdin)
    {
        return simulateFromTerminal(stream_in, stream_out, stack);
    }
    return simulateFromFile(stream_in, stream_out, stack);
}

Status simulateFromTerminal(FILE* stream_in, FILE* stream_out, Stack** stack)
{
    myAssert(stack      != nullptr);
    myAssert(stream_in  != nullptr);
    myAssert(stream_out != nullptr);

    Status status = OK;

    char  buffer[MAX_LINE_LEN]          = {};
    char* arguments_array[MAX_ARGS_NUM] = {};

    while (fgets(buffer, MAX_LINE_LEN - 1, stream_in) != NULL)
    {
        //NOTE debug print
        colorPrint(YELLOW, "Got line %s", buffer);

        size_t num_of_arguments = 1;
        myStringCount(buffer, SEPARATOR, &num_of_arguments);

        myStringReplace(buffer, '\n',      '\0');
        myStringReplace(buffer, SEPARATOR, '\0');

        //NOTE debug print
        colorPrint(YELLOW, "Line got %zu arguments \n", num_of_arguments);

        if (num_of_arguments > MAX_ARGS_NUM)
        {
            return SIM_NUM_OF_ARGS_ERR;
        }

        linkStringPointers(buffer, num_of_arguments, arguments_array);

        status = parseStackCommands(arguments_array,      num_of_arguments,
                                    stack_commands_array, stack_commands_array_size, stack, stream_out);
        returnIfError(status);
    }
    return OK;
}

Status simulateFromFile(FILE* stream_in, FILE* stream_out, Stack** stack)
{
    myAssert(stack      != nullptr);
    myAssert(stream_in  != nullptr);
    myAssert(stream_out != nullptr);

    Status status = OK;

    size_t file_text_size = 0;
    char   file_text[MAX_FILE_SIZE] = {};

    status = readDataFromFile(stream_in, file_text, &file_text_size);
    returnIfError(status);

    file_text[file_text_size] = '\0';

    size_t num_of_lines = 0;
    myStringCount(file_text, '\n', &num_of_lines);

    colorPrint(YELLOW, "File got %zu lines \n", num_of_lines);

    myStringReplace(file_text, '\n', '\0');

    char* file_lines_array[MAX_LINE_NUM] = {};

    linkStringPointers(file_text, num_of_lines, file_lines_array);

    char* arguments_array[MAX_ARGS_NUM] = {};

    for (size_t line_index = 0; line_index < num_of_lines; ++line_index)
    {
        //NOTE debug print
        colorPrint(YELLOW, "Got line %s \n", file_lines_array[line_index]);

        size_t num_of_arguments = 1;
        myStringCount(file_lines_array[line_index], SEPARATOR, &num_of_arguments);

        myStringReplace(file_lines_array[line_index], SEPARATOR, '\0');

        linkStringPointers(file_lines_array[line_index], num_of_arguments, arguments_array);

        //NOTE debug print
        colorPrint(YELLOW, "Line got %zu arguments \n", num_of_arguments);

        if (num_of_arguments > MAX_ARGS_NUM)
        {
            return SIM_NUM_OF_ARGS_ERR;
        }

        status = parseStackCommands(arguments_array,      num_of_arguments,
                                    stack_commands_array, stack_commands_array_size, stack, stream_out);
        returnIfError(status);
    }
    return OK;
}

Status parseStackCommands(char**        args_array,     size_t args_array_size,
                          StackCommand* commands_array, size_t commands_array_size, Stack** stack, FILE* stream_out)
{
    myAssert(stack          != nullptr);
    myAssert(args_array     != nullptr);
    myAssert(commands_array != nullptr);
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

Status readDataFromFile(FILE* stream_in, char* array, size_t* array_size)
{
    myAssert(stream_in  != nullptr);
    myAssert(array      != nullptr);
    myAssert(array_size != nullptr);

    Status status = OK;

    status = getFileSize(stream_in, array_size);
    returnIfError(status);
    if (*array_size >= MAX_FILE_SIZE - 1)
    {
        return FILE_SIZE_ERR;
    }

    size_t read = fread(array, sizeof(char), *array_size, stream_in);
    if (read != *array_size)
    {
        return FILE_READ_ERR;
    }
    return status;
}

Status getFileSize(FILE* stream_in, size_t* file_size)
{
    myAssert(stream_in != nullptr);
    myAssert(file_size != nullptr);

    long initial_position = ftell(stream_in);

    fseek(stream_in, 0, SEEK_END);
    long end_position = ftell(stream_in);
    if (end_position < 0 || end_position - initial_position < 0)
    {
        return FILE_SEEK_ERR;
    }
    fseek(stream_in, initial_position, SEEK_SET);

    *file_size = (size_t)(end_position - initial_position);
    return OK;
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
                //NOTE debug print
                colorPrint(YELLOW, "Parsed arg \"%s\" \n", (string + (string_index - current_length)));

                arguments_array[arg_index] = (string + (string_index - current_length));
                current_length = 0;
            }
            arg_index++;
        }
        else
        {
            current_length++;
        }
    }
    return OK;
}

Status myStringReplace(char* string, char old_symbol, char new_symbol)
{
    myAssert(string != nullptr);

    for (size_t index = 0; string[index] != '\0'; ++index)
    {
        if (string[index] == old_symbol)
        {
            string[index] = new_symbol;
        }
    }
    return OK;
}

Status myStringCount(char* string, char symbol, size_t* count)
{
    myAssert(string != nullptr);
    myAssert(count  != nullptr);

    for (size_t index = 0; string[index] != '\0'; ++index)
    {
        if (string[index] == symbol)
        {
            (*count)++;
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

Status setStackType(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
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

    //NOTE debug print
    colorPrint(YELLOW, "Parsed type %c of len %i \n", *type_identifier, type_len);

    if (type_identifier == nullptr || type_len > 1)
    {
        return SIM_STACK_TYPE_ERR;
    }

    bool found = false;
    for (size_t type_index = 0; type_index < stack_type_functions_array_size; ++type_index)
    {
        if (stack_type_functions_array[type_index].type_id == *type_identifier)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        return SIM_UNKNOWN_TYPE_ERR;
    }
    stack_data_type = *type_identifier;

    fprintf(stream_out, "Set stack data type %c \n", stack_data_type);

    return OK;
}

Status simulateCtor(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    if (*stack != nullptr)
    {
        fprintf(stream_out, "Stack already constructed, did nothing \n");
        return OK;
    }

    Status status = OK;

    if (!stack_data_type)
    {
        return SIM_NO_TYPE_ERR;
    }

    bool found = false;
    for (size_t type_index = 0; type_index < stack_type_functions_array_size; ++type_index)
    {
        if (stack_type_functions_array[type_index].type_id == stack_data_type)
        {
            fprintf(stream_out, "Called stack construct \n");

            status = stack_type_functions_array[type_index].ctor_func(stack);
            destroyIfError(status, stack);
            returnIfError(status);

            fprintf(stream_out, "Constructed stack of type \"%c\" \n", stack_data_type);

            found = true;
            break;
        }
    }
    if (!found)
    {
        return SIM_WRONG_TYPE_ERR;
    }
    return status;
}

Status simulatePush(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
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

    bool found = false;
    for (size_t type_index = 0; type_index < stack_type_functions_array_size; ++type_index)
    {
        if (stack_type_functions_array[type_index].type_id == stack_data_type)
        {
            fprintf(stream_out, "Called stack push \n");

            status = stack_type_functions_array[type_index].push_func(*stack, string_value);
            destroyIfError(status, stack);
            returnIfError(status);

            fprintf(stream_out, "Pushed value \"");

            void* pushed_value = NULL;
            status = stackPeek(*stack, &pushed_value);
            destroyIfError(status, stack);
            returnIfError(status);

            status = stackPrintElem(*stack, pushed_value, stream_out);
            destroyIfError(status, stack);
            returnIfError(status);

            fprintf(stream_out, "\" \n");

            found = true;
            break;
        }
    }
    if (!found)
    {
        return SIM_WRONG_TYPE_ERR;
    }
    return status;
}

Status simulatePop(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    if (!stack_data_type)
    {
        return SIM_NO_TYPE_ERR;
    }

    bool found = false;
    for (size_t type_index = 0; type_index < stack_type_functions_array_size; ++type_index)
    {
        if (stack_type_functions_array[type_index].type_id == stack_data_type)
        {
            fprintf(stream_out, "Called stack pop \n");

            void* variable = NULL;
            status = stack_type_functions_array[type_index].pop_func(*stack, &variable);
            destroyIfError(status, stack);
            returnIfError(status);

            fprintf(stream_out, "Popped value \"");

            status = stackPrintElem(*stack, variable, stream_out);
            destroyIfError(status, stack);
            returnIfError(status);

            fprintf(stream_out, "\" \n");

            found = true;
            break;
        }
    }
    if (!found)
    {
        return SIM_WRONG_TYPE_ERR;
    }
    return status;
}

Status simulatePeek(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    if (!stack_data_type)
    {
        return SIM_NO_TYPE_ERR;
    }

    bool found = false;
    for (size_t type_index = 0; type_index < stack_type_functions_array_size; ++type_index)
    {
        if (stack_type_functions_array[type_index].type_id == stack_data_type)
        {
            fprintf(stream_out, "Called stack pop \n");

            void* variable = NULL;
            status = stack_type_functions_array[type_index].peek_func(*stack, &variable);
            destroyIfError(status, stack);
            returnIfError(status);

            if (variable == NULL)
            {
                fprintf(stream_out, "Stack is empty \n");
            }
            else
            {
                fprintf(stream_out, "Peeked value \"");

                status = stackPrintElem(*stack, variable, stream_out);
                destroyIfError(status, stack);
                returnIfError(status);

                fprintf(stream_out, "\" \n");
            }

            found = true;
            break;
        }
    }
    if (!found)
    {
        return SIM_WRONG_TYPE_ERR;
    }
    return status;
}

Status simulateDump(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackDump(*stack, status);
    destroyIfError(status, stack);

    fprintf(stream_out, "Called stack dump \n");

    return status;
}

Status simulateVerify(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackVerify(*stack);
    destroyIfError(status, stack);

    fprintf(stream_out, "Called stack verify \n");

    return status;
}

Status simulateDtor(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackDestructor(*stack);

    fprintf(stream_out, "Called stack destructor \n");

    return status;
}

Status simulateFit(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackFitToSize(*stack);
    destroyIfError(status, stack);

    fprintf(stream_out, "Called stack fit to size \n");

    return status;
}

Status simulateClear(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    Status status = OK;

    status = stackClear(*stack);
    destroyIfError(status, stack);

    fprintf(stream_out, "Called stack clear \n");

    return status;
}

Status simulateGetSize(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    size_t stack_size = stackGetSize(*stack);

    fprintf(stream_out, "Called stack get size \n");
    fprintf(stream_out, "Stack size is %zu \n", stack_size);

    return OK;
}

Status simulateGetCap(Stack** stack, char** args_array, size_t args_array_size, size_t* arg_index, FILE* stream_out)
{
    unused(args_array); unused(args_array_size); unused(arg_index);

    myAssert(stack      != nullptr);
    myAssert(*stack     != nullptr);
    myAssert(args_array != nullptr);
    myAssert(stream_out != nullptr);
    myAssert(arg_index  != nullptr);

    size_t stack_capacity = stackGetCapacity(*stack);

    fprintf(stream_out, "Called stack get capacity \n");
    fprintf(stream_out, "Stack capacity is %zu \n", stack_capacity);

    return OK;
}

Status ctorInt(Stack** stack)
{
    myAssert(stack != nullptr);

    return stackConstructor(stack, DEFAULT_CAPACITY, printInt, NULL, NULL);
}

Status ctorChar(Stack** stack)
{
    myAssert(stack != nullptr);

    return stackConstructor(stack, DEFAULT_CAPACITY, printChar, NULL, NULL);
}

Status ctorLong(Stack** stack)
{
    myAssert(stack != nullptr);

    return stackConstructor(stack, DEFAULT_CAPACITY, printLong, NULL, NULL);
}

Status ctorStr(Stack** stack)
{
    myAssert(stack != nullptr);

    return stackConstructor(stack, DEFAULT_CAPACITY, printStr, cloneStr, freeStr);
}

Status pushInt(Stack* stack, const char* string)
{
    myAssert(stack  != nullptr);

    Status status = OK;

    const void* value = (const void*)((uintptr_t)((int)strtol(string, NULL, 10)));
    status = stackPush(stack, value);

    return status;
}

Status pushChar(Stack* stack, const char* string)
{
    myAssert(stack  != nullptr);

    Status status = OK;

    const void* value = (const void*)((uintptr_t)(*string));
    status = stackPush(stack, value);

    return status;
}

Status pushLong(Stack* stack, const char* string)
{
    myAssert(stack  != nullptr);

    Status status = OK;

    const void* value = (const void*)((uintptr_t)strtol(string, NULL, 10));
    status = stackPush(stack, value);

    return status;
}

Status pushStr(Stack* stack, const char* string)
{
    myAssert(stack  != nullptr);

    Status status = OK;

    const void* value = (const void*)string;
    status = stackPush(stack, value);

    return status;
}

Status peekVar(Stack* stack, void** variable)
{
    myAssert(stack    != nullptr);
    myAssert(variable != nullptr);

    Status status = OK;

    status = stackPeek(stack, variable);

    return status;
}

Status popVar(Stack* stack, void** variable)
{
    myAssert(stack    != nullptr);
    myAssert(variable != nullptr);

    Status status = OK;

    status = stackPop(stack, variable);

    return status;
}
