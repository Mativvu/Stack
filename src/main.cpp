#include <stdio.h>
#include <string.h>

#include "FlagParser.h"
#include "Commands.h"
#include "Errors.h"
#include "Color.h"
#include "Debug.h"
//#include "StackVoid.h"
#include "StackNorm.h"

//NOTE add assert info from previous function to next function?
//TODO make checkValue for stack push
//NOTE rename errors?
//TODO commands array extern change, parser get command array as arg

void print_int(const void* stack_elem)
{
    printf("%p \n", stack_elem);
}

void print_str(const void* stack_elem)
{
    printf("%s \n", (char*)stack_elem);
}

void* clone_str(const void* buf)
{
    return strdup((char*)buf);
}

int main(const int argc, const char** argv)
{
    int modes_bitset = 0;
    Status status    = OK;

    FlagParseData ParsedData = {};
    status = parseFlags(argc, argv, &modes_bitset, &ParsedData, commands_array, commands_array_size);
    checkStatus(status);
    debugPrintString("#1 Initialization completed, files opened successfully \n");

    Stack* stack = nullptr;

    status = stackConstructor(&stack, 10);
    checkStatus(status);
    debugPrintString("#2 Stack constructed \n");

    status = stackVerify(stack);
    checkStatus(status);
    debugPrintString("#3 Stack verified \n");

    status = stackPush(stack, 3.1415);
    status = stackPush(stack, 4.1415);
    status = stackPush(stack, 5.1415);
    status = stackPush(stack, 6.1415);
    status = stackPush(stack, 7.1415);
    status = stackPush(stack, 8.1415);
    checkStatus(status);
    debugPrintString("#4 Pushed value \n");

    status = stackPush(stack, 25.3);
    checkStatus(status);
    debugPrintString("#5 Pushed value \n");

    cprintf(YELLOW, "Stack capacity: %zu\n", stackGetCapacity(stack));

    elem_t temp = NULL;
    status = stackPop(stack, &temp);
    checkStatus(status);
    debugPrintString("#6 Popped value \n");

    cprintf(YELLOW, "Popped \"%" ELEM_T "\"\n", temp);

    //TODO: add situations

    status = stackDump(stack, status);
    checkStatus(status);
    debugPrintString("#7 Called dump \n");

    status = stackDestructor(stack);
    checkStatus(status);
    debugPrintString("#N Stack destroyed \n");
}
