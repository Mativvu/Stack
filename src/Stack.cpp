#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Stack.h"
#include "Debug.h"

struct myStack
{
    ull         canary_left;
    void**      data;
    size_t      size;
    size_t      capacity;
    print_func  print;
    clone_func  clone;
    free_func   free;
    ull         canary_right;
};

Status stackConstructor(Stack** stack, size_t capacity, print_func print, clone_func clone, free_func free)
{
    myAssert(stack != nullptr);

    Status status = OK;

    if (print == nullptr)
    {
        return NO_PRINT_FUNC_ERR;
    }
    if (0 >= capacity || capacity > MAX_CAPACITY)
    {
        return CAPACITY_VALUE_ERR;
    }

    *stack = (Stack*)malloc(sizeof(Stack));
    if (*stack == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    **stack = (Stack){CANARY_LEFT, nullptr, ZERO, capacity, print, clone, free, CANARY_RIGHT};

    (*stack)->data = (void**)calloc((capacity + 2), sizeof(void*));
    if ((*stack)->data == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    ((ull*) (*stack)->data)[0]             = CANARY_LEFT;
    ((ull*)((*stack)->data + capacity))[1] = CANARY_RIGHT;

    //NOTE debug print
    //colorPrint(YELLOW, "Ctor left canary:  0x%llX \n", ((ull*)(*stack)->data)[0]);
    //colorPrint(YELLOW, "Ctor right canary: 0x%llX \n", ((ull*)((*stack)->data + capacity))[1]);

    status = stackVerify(*stack);
    returnIfError(status);

    return OK;
}

Status stackPop(Stack* stack, void** variable)
{
    myAssert(stack       != nullptr);
    myAssert(stack->data != nullptr);
    myAssert(variable    != nullptr);

    Status status = OK;

    status = stackVerify(stack);
    returnIfError(status);

    if (stack->size <= ZERO)
    {
        return STACK_UNDERFLOW_ERR;
    }
    *variable = stack->data[stack->size];
    stack->data[stack->size] = NULL;
    stack->size--;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackPush(Stack* stack, const void* value)
{
    myAssert(stack       != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;

    status = stackVerify(stack);
    returnIfError(status);

    if (stack->size >= stack->capacity)
    {
        status = stackResize(stack, stack->capacity * 2);
        returnIfError(status);
    }

    stack->data[stack->size + 1] = stack->clone ? stack->clone(value) : (void*)value;
    stack->size++;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackDump(Stack* stack, Status error)
{
    myAssert(stack       != nullptr);
    myAssert(stack->data != nullptr);

    FILE* dump_stream = fopen(DUMP_FILE_PATH, "w");
    if (dump_stream == nullptr)
    {
        return DUMP_FILE_ERR;
    }

    fprintf(dump_stream, "Dump called with error: \n");
    printErrorMessage(dump_stream, error);
    fprintf(dump_stream, "\n");

    fprintf(dump_stream, "Stack address: %p \n",          stack);
    fprintf(dump_stream, "Stack capacity = %zu \n",       stack->capacity);
    fprintf(dump_stream, "Stack size = %zu \n",           stack->size);
    fprintf(dump_stream, "Stack left  canary: 0x%llX \n", stack->canary_left);
    fprintf(dump_stream, "Stack right canary: 0x%llX \n", stack->canary_right);
    fprintf(dump_stream, "\n");

    fprintf(dump_stream, "Stack data address: %p \n", stack->data);
    fprintf(dump_stream, "Stack data: \n");
    if (stack->size == ZERO)
    {
        fprintf(dump_stream, "Stack is empty \n");
    }
    for (size_t index = stack->size; index >= 1; --index)
    {
        fprintf(dump_stream, "[%zu] \t", index - 1);
        stack->print(dump_stream, stack->data[index]);
    }

    fprintf(dump_stream, "Data left  canary: 0x%llX \n", ((ull*)(stack->data))[0]);
    fprintf(dump_stream, "Data right canary: 0x%llX \n", ((ull*)(stack->data + stack->capacity))[1]);

    return OK;
}

Status stackDestructor(Stack* stack)
{
    myAssert(stack       != nullptr);

    if (stack->data != nullptr)
    {
        for (size_t index = 0; index < stack->capacity + 2; ++index)
        {
            stack->data[index] = NULL;
            if (stack->free)
            {
                stack->free(stack->data[index]);
                stack->data[index] = nullptr;
            }
        }
    }

    free(stack->data);
    stack->data = nullptr;

    stack->capacity = POISON_SIZE_T;
    stack->size     = POISON_SIZE_T;

    return OK;
}

Status stackVerify(Stack* stack)
{
    myAssert(stack       != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;

    if (0 >= stack->capacity || stack->capacity > MAX_CAPACITY)
    {
        status = CAPACITY_VALUE_ERR;
    }
    else if (stack->capacity < stack->size)
    {
        status = STACK_OVERFLOW_ERR;
    }
    else if (stack->size < ZERO)
    {
        status = STACK_UNDERFLOW_ERR;
    }
    else if (stack->capacity == POISON_SIZE_T || stack->size == POISON_SIZE_T)
    {
        status = POISON_VALUE_ERR;
    }
    else if (((ull*)(stack->data))[0]                   != CANARY_LEFT ||
             ((ull*)(stack->data + stack->capacity))[1] != CANARY_RIGHT)
    {
        status = STACK_DATA_DAMAGED;
    }
    else if(stack->canary_left  != CANARY_LEFT ||
            stack->canary_right != CANARY_RIGHT)
    {
        status = STACK_FIELD_DAMAGED;
    }

    if (isError(status))
    {
        stackDump(stack, status);
    }
    return status;
}

Status stackResize(Stack* stack, const size_t new_capacity)
{
    myAssert(stack       != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;

    status = stackVerify(stack);
    returnIfError(status);

    //NOTE debug print
    colorPrint(YELLOW, "Called resize \n");

    ((ull*)(stack->data + stack->capacity))[1] = ZERO;

    if (new_capacity >= MAX_CAPACITY)
    {
        return CAPACITY_VALUE_ERR;
    }
    else
    {
        stack->capacity = new_capacity;
    }

    stack->data = (void**)realloc(stack->data, (stack->capacity + 2) * sizeof(void*));
    if (stack->data == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    ((ull*)(stack->data + stack->capacity))[1] = CANARY_RIGHT;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackPeek(Stack* stack, void** variable)
{
    myAssert(stack       != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;

    status = stackVerify(stack);
    returnIfError(status);

    if (stack->size == ZERO)
    {
        *variable = NULL;
    }
    else
    {
        *variable = stack->data[stack->size];
    }

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackFitToSize(Stack* stack)
{
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;
    status = stackVerify(stack);
    returnIfError(status);

    ((ull*)(stack->data + stack->capacity))[1] = ZERO;

    stack->capacity = stack->size;

    stack->data = (void**)realloc(stack->data, (stack->capacity + 2) * sizeof(void*));
    if (stack->data == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    ((ull*)(stack->data + stack->capacity))[1] = CANARY_RIGHT;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackClear(Stack* stack)
{
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;

    status = stackVerify(stack);
    returnIfError(status);

    for (size_t index = stack->capacity; index > ZERO; index--)
    {
        if (stack->free)
        {
            free(stack->data[index]);
        }
        stack->data[index] = NULL;
    }
    stack->size = ZERO;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

size_t stackGetCapacity(Stack* stack)
{
    myAssert(stack != nullptr);

    return stack->capacity;
}

size_t stackGetSize(Stack* stack)
{
    myAssert(stack != nullptr);

    return stack->size;
}

Status stackPrintElem(Stack* stack, const void* value, FILE* stream)
{
    myAssert(stack != nullptr);

    stack->print(stream, value);

    return OK;
}

void printStr(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%s", (char*)stack_elem);
}

void printInt(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%d", (int)((uintptr_t) stack_elem));
}

void printChar(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%c", (char)((uintptr_t) stack_elem));
}

void printLong(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%lld", (long long)((uintptr_t) stack_elem));
}

void* cloneStr(const void* buf)
{
    return strdup((char*)buf);
}

void freeStr(void* stack_elem)
{
    free(stack_elem);
}

