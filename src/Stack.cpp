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
    Status status = OK;

    if (print == nullptr)
    {
        return NO_PRINT_FUNC_ERR;
    }

    *stack = (Stack*)malloc(sizeof(Stack));
    if (*stack == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    if (capacity > MAX_CAPACITY)
    {
        return CAPACITY_LIMIT_ERR;
    }
    **stack = (Stack){CANARY_LEFT, nullptr, ZERO, capacity, print, clone, free, CANARY_RIGHT};

    (*stack)->data = (void**)malloc((capacity + 2) * sizeof(void*));
    if ((*stack)->data == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    ((ull*)(*stack)->data)[0]              = CANARY_LEFT;
    ((ull*)((*stack)->data + capacity))[1] = CANARY_RIGHT;

    //NOTE
    //cprintf(YELLOW, "Ctor left canary:  0x%llX \n", ((ull*)(*stack)->data)[0]);
    //cprintf(YELLOW, "Ctor right canary: 0x%llX \n", ((ull*)((*stack)->data + capacity))[1]);

    status = stackVerify((*stack));
    returnIfError(status);

    return OK;
}

Status stackPop(Stack* stack, void** variable)
{
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);
    myAssert(variable != nullptr);

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
    myAssert(stack != nullptr);
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
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    FILE* dump_stream = fopen(DUMP_FILE_PATH, "w");
    if (dump_stream == nullptr)
    {
        return DUMP_FILE_ERR;
    }

    char* error_message = describeError(error);
    fprintf(dump_stream, "Dump called with error:\n%s", error_message);
    fprintf(dump_stream, "\n");
    free(error_message);

    fprintf(dump_stream, "Stack address: %p \n", stack);
    fprintf(dump_stream, "Stack capacity = %zu \n", stack->capacity);
    fprintf(dump_stream, "Stack size = %zu \n",     stack->size);
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

    return DUMPED;
}

Status stackDestructor(Stack* stack)
{
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    for (size_t index = 0; index < stack->capacity + 2; ++index)
    {
        stack->data[index] = NULL;
        if (stack->free)
        {
            stack->free(stack->data[index]);
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
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;

    if (stack->capacity > MAX_CAPACITY)
    {
        status = CAPACITY_LIMIT_ERR;
    }
    else if (stack->capacity < stack->size)
    {
        status = STACK_OVERFLOW_ERR;
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

Status stackResize(Stack* stack, size_t new_capacity)
{
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;
    status = stackVerify(stack);
    returnIfError(status);

    cprintf(YELLOW, "Called resize \n");

    ((ull*)(stack->data + stack->capacity))[1] = ZERO;

    if (new_capacity >= MAX_CAPACITY)
    {
        return CAPACITY_LIMIT_ERR;
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
    myAssert(stack != nullptr);
    myAssert(stack->data != nullptr);

    Status status = OK;
    status = stackVerify(stack);
    returnIfError(status);

    if (stack->size <= ZERO)
    {
        return STACK_UNDERFLOW_ERR;
    }
    *variable = stack->data[stack->size];

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

    for (size_t index = stack->capacity + 1; index > ZERO; index--)
    {
        if (stack->free)
        {
            free(stack->data[index]);
        }
        stack->data[index] = NULL;
    }

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

void print_str(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%s \n", (char*)stack_elem);
}

void print_int(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%d \n", (int)((uintptr_t) stack_elem));
}

void print_char(FILE* stream, const void* stack_elem)
{
    fprintf(stream, "%c \n", (char)((uintptr_t) stack_elem));
}

void* clone_str(const void* buf)
{
    return strdup((char*)buf);
}

void free_str(void* stack_elem)
{
    free(stack_elem);
}

