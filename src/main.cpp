#include <stdio.h>

#include "FlagParser.h"
#include "Commands.h"
#include "Errors.h"
#include "Color.h"
#include "Debug.h"
#include "Stack.h"
#include "StackSim.h"

//NOTE add assert info from previous function to next function?
//TODO make checkValue for stack push
//NOTE rename errors?
//TODO add hash protection
//TODO add function to filter incoming values
//TODO add function to transform incoming value before pushing in stack
//TODO add stack simulation from file
//TODO add error for not enough arguments for command

int main(const int argc, const char** argv)
{
    int modes_bitset = 0;
    Status status    = OK;

    FlagParseData ParsedData = {};
    status = parseFlags(argc, argv, &modes_bitset, &ParsedData, commands_array, commands_array_size);
    checkStatus(status);
    debugPrintString("#1 Initialization completed, files opened successfully \n");

    FILE* stream_in  = ParsedData.stream_in;
    FILE* stream_out = ParsedData.stream_out;

    Stack* stack = nullptr;

    status = stackConstructor(&stack, 1, print_int, NULL, NULL);
    checkStatus(status);
    debugPrintString("#2 Stack constructed \n");

    status = stackSimulation(stream_in, stream_out, stack);
    checkStatus(status);
    debugPrintString("#3 Stack simulation ended successfully \n");

    status = stackDestructor(stack);
    checkStatus(status);
    debugPrintString("#N Stack destroyed \n");
}
