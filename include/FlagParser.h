#ifndef FLAGPARSER_H
#define FLAGPARSER_H

#include "Errors.h"
#include "Commands.h"

Status parseFlags(const int argc, const char** argv, int* modes_bitset,
                  FlagParseData* ParsedData, Command* commands_array, size_t commands_array_size);

bool isAllModesSet(const int modes_bitset, const int modes_bits);
bool isAnyModesSet(const int modes_bitset, const int modes_bits);
bool isModeSet    (const int modes_bitset, const int mode_bit);
bool isCommand    (const char* str, const Command* command);

#endif //FLAGPARSER_H
