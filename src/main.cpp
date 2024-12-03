#include <stdio.h>

#include "FlagParser.h"
#include "Commands.h"
#include "Errors.h"
#include "Color.h"
#include "Debug.h"
#include "Stack.h"
#include "StackSim.h"

//NOTE add assert info from previous function to next function?
//NOTE rename errors?
//TODO add hash protection
//TODO add error for not enough arguments for command
//TODO change const 2 to const name (CANARY_NUM?)
//TODO try to make constructor in sim
//TODO solve problem with switch cases because of type id`s
//TODO separate stack simulation from file and from terminal

int main(const int argc, const char** argv)
{
    int modes_bitset = 0;
    Status status    = OK;

    FlagParseData ParsedData = {};
    status = parseFlags(argc, argv, &modes_bitset, &ParsedData, commands_array, commands_array_size);
    checkStatus(status);
    debugPrint("#1 Initialization completed, files opened successfully \n");

    FILE* stream_in  = ParsedData.stream_in;
    FILE* stream_out = ParsedData.stream_out;

    Stack* stack = nullptr;

    /*
    status = stackConstructor(&stack, 1, printInt, NULL, NULL);
    checkStatus(status);
    debugPrint("#2 Stack constructed \n");
    */

    status = stackSimulation(stream_in, stream_out, &stack);
    checkStatus(status);
    debugPrint("#3 Stack simulation ended successfully \n");

    status = stackDestructor(stack);
    checkStatus(status);
    debugPrint("#N Stack destroyed \n");
}
