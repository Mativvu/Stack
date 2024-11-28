#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Commands.h"
#include "Color.h"
#include "Debug.h"

Command commands_array[] =
{
    {HELP,    printCommands,     "--help",  "-h", "you already know, don`t you?)\n"},
    {DEFAULT, setDefaultStreams, "--file",  "-f", "default working mode, extracts data from \"input.txt\""
                                                  "and prints in \"output.txt\" \n"},
    {INPUT,   setInputStream,    "--in",    "-i", "type \"filename\" after it to input from \"filename\","
                                                  "file must exist in folder \"files/input\" \n"},
    {OUTPUT,  setOutputStream,   "--out",   "-o", "type \"filename\" after it to output in \"filename\","
                                                  "file must exist in folder \"files/output\" \n"},
    {APPEND,  setAppendStream,   "--app",   "-a", "type \"filename\" after it to input from \"filename\""
                                                  " and output in the end of it, file must exist in"
                                                  "folder \"files/input\" \n"}
};
size_t commands_array_size = sizeof(commands_array)/sizeof(commands_array[0]);

Status setInputStream(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData)
{
    myAssert(arg_index != nullptr);
    myAssert(ParsedData != nullptr);

    Status status = OK;

    FILE** stream_in = &ParsedData->stream_in;
    char* file_path = nullptr;

    if (*stream_in != nullptr)
    {
        return STREAM_REUSE_ERR;
    }

    const char* file_name = getNextArgument(argc, argv, arg_index);
    if (file_name == nullptr)
    {
        return FILE_OPEN_ERR;
    }

    status = createFilePath(&file_path, FILE_DIR, file_name);
    returnIfError(status);

    *stream_in = fopen(file_path, "r");
    if (*stream_in == nullptr)
    {
        return FILE_OPEN_ERR;
    }
    return OK;
}

Status setOutputStream(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData)
{
    myAssert(arg_index != nullptr);
    myAssert(ParsedData  != nullptr);

    Status status = OK;

    FILE** stream_out = &ParsedData->stream_out;
    char* file_path = nullptr;

    if (*stream_out != nullptr)
    {
        return STREAM_REUSE_ERR;
    }

    const char* file_name = getNextArgument(argc, argv, arg_index);
    if (file_name == nullptr)
    {
        return FILE_OPEN_ERR;
    }

    status = createFilePath(&file_path, FILE_DIR, file_name);
    returnIfError(status);

    *stream_out = fopen(file_path, "w");
    if (*stream_out == nullptr)
    {
        return FILE_OPEN_ERR;
    }
    return OK;
}

Status setAppendStream(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData)
{
    myAssert(arg_index != nullptr);
    myAssert(ParsedData  != nullptr);

    Status status = OK;

    FILE** stream_in =  &ParsedData->stream_in;
    FILE** stream_out = &ParsedData->stream_out;
    char* file_path = nullptr;

    if (*stream_out != nullptr || *stream_in != nullptr)
    {
        return STREAM_REUSE_ERR;
    }

    const char* file_name = getNextArgument(argc, argv, arg_index);
    if (file_name == nullptr)
    {
        return FILE_OPEN_ERR;
    }

    status = createFilePath(&file_path, FILE_DIR, file_name);
    returnIfError(status);

    *stream_in  = fopen(file_path, "a+");
    *stream_out = fopen(file_path, "a+");
    if (*stream_out == nullptr || *stream_in == nullptr)
    {
        return FILE_OPEN_ERR;
    }
    return OK;
}

Status setDefaultStreams(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData)
{
    unused(argc);
    unused(argv);
    unused(arg_index);
    myAssert(ParsedData  != nullptr);

    Status status = OK;

    FILE** stream_in  = &ParsedData->stream_in;
    FILE** stream_out = &ParsedData->stream_out;

    if (*stream_in != nullptr || *stream_out != nullptr)
    {
        return STREAM_REUSE_ERR;
    }

    *stream_in  = fopen(DEFAULT_INPUT_FILE_PATH,  "r");
    *stream_out = fopen(DEFAULT_OUTPUT_FILE_PATH, "w");
    if (*stream_in == nullptr || *stream_out == nullptr)
    {
        return FILE_OPEN_ERR;
    }
    return OK;
}

Status printCommands(const int argc, const char** argv, int* arg_index, FlagParseData* ParsedData)
{
    unused(argc);
    unused(argv);
    unused(arg_index);
    unused(ParsedData);

    printf("\n");
    for (size_t i = 0; i < commands_array_size; i++)
    {
        cprintf(CYAN, "%s ", commands_array[i].name);
        cprintf(MAGENTA, "or ");
        cprintf(CYAN, "%s ", commands_array[i].short_name);
        cprintf(MAGENTA, "%s\n", commands_array[i].description);
    }
    return HELP_FLAG;
}

Status createFilePath(char** file_path, const char* dir, const char* file_name)
{
    myAssert(file_path != nullptr);
    myAssert(file_name != nullptr);
    myAssert(dir != nullptr);

    long path_length = snprintf(NULL, 0, "%s%s", dir, file_name) + 1;
    if (path_length <= 0 || (size_t)path_length >= MAX_NAME_LEN * 2)
    {
        return FILE_NAME_LEN_ERR;
    }

    *file_path = (char*)calloc(MAX_NAME_LEN * 2, sizeof(char));
    if (*file_path == nullptr)
    {
        return MEMORY_ALLOC_ERR;
    }

    long res = snprintf(*file_path, (size_t)path_length, "%s%s", dir, file_name) + 1;
    if (res != path_length)
    {
        return FILE_NAME_LEN_ERR;
    }
    return OK;
}

const char* getNextArgument(const int argc, const char** argv, int* arg_index)
{
    if (*arg_index + 1 < argc)
    {
        (*arg_index)++;

        return argv[*arg_index];
    }
    return nullptr;
}

void freeAndCloseParsed(FlagParseData* ParsedData)
{
    fclose(ParsedData->stream_in);
    fclose(ParsedData->stream_out);
}
