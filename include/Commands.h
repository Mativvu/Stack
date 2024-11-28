#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

#include "Errors.h"

const size_t MAX_NAME_LEN = 16;

const char* const FILE_DIR = "files/";

const char* const DEFAULT_INPUT_FILE_PATH  = "files/input.txt";
const char* const DEFAULT_OUTPUT_FILE_PATH = "files/output.txt";

#define unused(param) (void)(param);

//TODO: change modes

enum Mode_bit
{
    DEFAULT = 0x00000001,
    DEBUG   = 0x00000010,
    HELP    = 0x00000100,
    INPUT   = 0x00001000,
    OUTPUT  = 0x00010000,
    APPEND  = 0x00011000
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

Status setDefaultStreams(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setAppendStream  (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setOutputStream  (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status setInputStream   (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);
Status printCommands    (const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData);

const char* getNextArgument(const int argc, const char** argv, int* arg_index);

Status createFilePath(char** file_path, const char* const dir, const char* file_name);

void freeAndCloseParsed(FlagParseData* ParsedData);

#endif //COMMANDS_H
