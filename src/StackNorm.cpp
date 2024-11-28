#include <stdio.h>

#include "StackNorm.h"
#include "Debug.h"

struct myStack
{
    ull      canary_left;
    elem_t*  data;
    size_t   size;
    size_t   capacity;
    ull      canary_right;
};

Status stackConstructor(Stack** stack, size_t capacity)
{
    Status status = OK;

    *stack = (Stack*)malloc(sizeof(Stack));
    if (*stack == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    if (capacity > MAX_CAPACITY)
    {
        return CAPACITY_LIMIT_ERR;
    }
    **stack = (Stack){CANARY_LEFT, nullptr, ZERO, capacity, CANARY_RIGHT};

    (*stack)->data = (elem_t*)calloc(capacity + 2*ELEMS_PER_CANARY, sizeof(elem_t));

    if ((*stack)->data == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    *((ull*)(*stack)->data)  = CANARY_LEFT;
    *((ull*)((*stack)->data + (capacity + ELEMS_PER_CANARY))) = CANARY_RIGHT;

    //NOTE
    //cprintf(YELLOW, "Ctor left canary:  %llu \n", ((ull*)(*stack)->data)[0]);
    //cprintf(YELLOW, "Ctor right canary: %llu \n", ((ull*)((*stack)->data + capacity))[1]);

    status = stackVerify((*stack));
    returnIfError(status);

    return OK;
}

Status stackPop(Stack* stack, elem_t* variable)
{
    myAssert(stack != nullptr);
    myAssert(variable != nullptr);

    Status status = OK;
    status = stackVerify(stack);
    returnIfError(status);

    if (stack->size <= ZERO)
    {
        return STACK_UNDERFLOW_ERR;
    }
    *variable = stack->data[stack->size + ELEMS_PER_CANARY - 1];
    stack->data[stack->size + ELEMS_PER_CANARY - 1] = NULL;
    stack->size--;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackPush(Stack* stack, const elem_t value)
{
    myAssert(stack != nullptr);

    Status status = OK;
    status = stackVerify(stack);
    returnIfError(status);

    if (stack->size >= stack->capacity)
    {
        status = stackResize(stack, stack->capacity * 2);
        returnIfError(status);
    }

    stack->data[stack->size + ELEMS_PER_CANARY] = value;
    stack->size++;

    status = stackVerify(stack);
    returnIfError(status);

    return OK;
}

Status stackDump(Stack* stack, Status error)
{
    myAssert(stack != nullptr);

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
    for (size_t index = ELEMS_PER_CANARY + stack->size - 1; index >= ELEMS_PER_CANARY; --index)
    {
        fprintf(dump_stream, "[%zu] \t %" ELEM_T " \n", index - ELEMS_PER_CANARY, stack->data[index]);
    }

    fprintf(dump_stream, "Data left  canary: 0x%llX \n", ((ull*)(stack->data))[0]);
    fprintf(dump_stream, "Data right canary: 0x%llX \n", ((ull*)(stack->data + stack->capacity))[1]);

    return DUMPED;
}

Status stackDestructor(Stack* stack)
{
    myAssert(stack != nullptr);

    for (size_t index = 0; index < stack->capacity + 2*ELEMS_PER_CANARY; ++index)
    {
        stack->data[index] = NULL;
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

    cprintf(YELLOW, "Called resize \n");

    ((ull*)(stack->data))[0] = ZERO;
    ((ull*)(stack->data + stack->capacity))[1] = ZERO;

    if (new_capacity >= MAX_CAPACITY)
    {
        return CAPACITY_LIMIT_ERR;
    }
    else
    {
        stack->capacity = new_capacity;
    }

    stack->data = (elem_t*)realloc(stack->data, (stack->capacity + 2*ELEMS_PER_CANARY) * sizeof(elem_t));
    if (stack->data == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    ((ull*)(stack->data))[0] = CANARY_LEFT;
    ((ull*)(stack->data + stack->capacity))[1] = CANARY_RIGHT;

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
