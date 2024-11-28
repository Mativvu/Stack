#include <stdio.h>

#include "FlagParser.h"
#include "Commands.h"
#include "Errors.h"
#include "Color.h"
#include "Debug.h"
#include "Stack.h"

//NOTE add assert info from previous function to next function?
//TODO make checkValue for stack push
//NOTE rename errors?
//TODO commands array extern change, parser get command array as arg

int main(const int argc, const char** argv)
{
    int modes_bitset = 0;
    Status status    = OK;

    FlagParseData ParsedData = {};
    status = parseFlags(argc, argv, &modes_bitset, &ParsedData, commands_array, commands_array_size);
    checkStatus(status);
    debugPrintString("#1 Initialization completed, files opened successfully \n");

    Stack* stack = nullptr;

    status = stackConstructor(&stack, 1, print_char, NULL, NULL);
    checkStatus(status);
    debugPrintString("#2 Stack constructed \n");

    status = stackVerify(stack);
    checkStatus(status);
    debugPrintString("#3 Stack verified \n");

    status = stackPush(stack, (const void*)'a');
    status = stackPush(stack, (const void*)'b');
    status = stackPush(stack, (const void*)'!');
    status = stackPush(stack, (const void*)'(');
    checkStatus(status);
    debugPrintString("#4 Pushed values \n");

    cprintf(YELLOW, "Stack capacity: %zu\n", stackGetCapacity(stack));

    void* temp = NULL;
    status = stackPop(stack, &temp);
    checkStatus(status);
    debugPrintString("#5 Popped value \n");

    //cprintf(YELLOW, "Popped \"%c\"\n", (char)((uintptr_t) temp));

    void* peek = NULL;
    status = stackPeek(stack, &peek);
    checkStatus(status);
    debugPrintString("#6 Peeked at stack \n");

    status = stackFitToSize(stack);
    checkStatus(status);
    debugPrintString("#7 Stack fitted \n");

    status = stackClear(stack);
    checkStatus(status);
    debugPrintString("#8 Stack cleared \n");

    status = stackDump(stack, status);
    checkStatus(status);
    debugPrintString("#9 Called dump \n");

    status = stackDestructor(stack);
    checkStatus(status);
    debugPrintString("#N Stack destroyed \n");
}
