#include <string>

#include "FlagParser.h"
#include "Color.h"
#include "Debug.h"

Status parseFlags(const int argc, const char** argv, int* modes_bitset,
                  FlagParseData* ParsedData, Command* commands_array, size_t commands_array_size)
{
    myAssert(modes_bitset != nullptr);
    myAssert(ParsedData != nullptr);

    Status status = OK;
    if (argc == 1)
    {
        colorPrint(MAGENTA, "Type --help to see commands\n");
    }
    for (int arg_index = 1; arg_index < argc; arg_index++)
    {
        bool found = false;
        for (size_t command_index = 0; command_index < commands_array_size; command_index++)
        {
            if (isCommand(argv[arg_index], &commands_array[command_index]))
            {
                if (isModeSet(*modes_bitset, commands_array[command_index].mode_bit))
                {
                    return ARG_DUPLICATION_ERR;
                }
                *modes_bitset |= commands_array[command_index].mode_bit;

                status = commands_array[command_index].function(argc, argv, &arg_index, ParsedData);
                returnIfError(status);

                found = true;
                break;
            }
        }
        if (!found)
        {
            colorPrint(MAGENTA, "Found unknown argument: %s\n", argv[arg_index]);
            return UNKNOWN_ARG_ERR;
        }
    }
    return OK;
}

bool isAllModesSet(const int modes_bitset, const int modes_bits)
{
    return ((modes_bitset & modes_bits) == modes_bits);
}

bool isAnyModesSet(const int modes_bitset, const int modes_bits)
{
    return (modes_bitset & modes_bits);
}

bool isModeSet(const int modes_bitset, const int mode_bit)
{
    return (modes_bitset & mode_bit);
}

bool isCommand(const char* str, const Command* command)
{
    return (strcmp(str, command->name)       == 0 ||
            strcmp(str, command->short_name) == 0);
}
