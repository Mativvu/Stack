#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

#include "Errors.h"

const size_t MAX_NAME_LEN = 16;

const char* const FILE_DIR = "files/";

const char* const DEFAULT_INPUT_FILE_PATH  = "files/input.txt";
const char* const DEFAULT_OUTPUT_FILE_PATH = "files/output.txt";

#define unused(param) while(0){ (void)(param); };

enum Mode_bit
{
    HELP     = 0b00000001,
    DEFAULT  = 0b00000010,
    INPUT    = 0b00000100,
    OUTPUT   = 0b00001000,
    APPEND   = 0b00010000,
    TERMINAL = 0b00100000
};

typedef struct FlagParseData
{
    FILE* stream_in;
    FILE* stream_out;
} FlagParseData;

typedef Status (*Command_function)(const int argc, const char** argv, int* arg_index,
                                   FlagParseData* ParsedData);

typedef struct Command
{
    const Mode_bit    mode_bit;
    Command_function  function;
    const char*       name;
    const char*       short_name;
    const char*       description;
} Command;

extern Command commands_array[];
extern size_t  commands_array_size;

Status setDefaultStreams (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setAppendStream   (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setOutputStream   (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setInputStream    (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setTerminalStreams(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status printCommands     (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);

const char* getNextArgument(const int argc, const char** argv, int* arg_index);

Status createFilePath(char** file_path, const char* const dir, const char* file_name);

void freeAndCloseParsed(FlagParseData* ParsedData);

#endif //COMMANDS_H
