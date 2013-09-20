#ifndef CONSTS_H_INCLUDED
#define CONSTS_H_INCLUDED


typedef std::map<std::string, long long> StrTo64Map_t;
typedef std::pair<std::string, int> StrTo32Pair_t;


enum ERRORS
{
    ERROR_SCRIPT_OPEN_FILE_CTOR = 5,
    ERROR_SCRIPT_OPEN_FILE_SAVE,
    ERROR_CONFLICTING_NAMES,
    ERROR_INVALID_HEADER,
    ERROR_VIRTUAL_PROC_CRASHED,
    ERROR_FATAL_RETURN,
    ERROR_NO_FUNC_IMPORT,
    ERROR_INVALID_VALUE_0,
    ERROR_NULL_SIZE_STRUCT,
    ERROR_MEMBER_NOT_FOUND,
    ERROR_ADDING_VAR_STRUCT,
    ERROR_INVALID_STRUCT_VAR_NUM_STRUCT,
    ERROR_PARSER_INVALID_ARGS,
    ERROR_NOT_RECOGNIZED_CMD,
    ERROR_VAR_ALREADY_EXISTS,
    ERROR_PARSER_CRASHED,
    ERROR_NESTED_FUNC,
    ERROR_NESTED_STRUCT,
    ERROR_STRUCT_UNEXPECTED_CMD,
    ERROR_LINK_UNEXPECTED_CMD,
    ERROR_UNDEFINED_REFERENCE_FUNC,
    ERROR_UNDEFINED_REFERENCE_LABEL,
    ERROR_INVALID_UNREF,
    ERROR_IMPORT_DLL_INVALID_STR,
    ERROR_NOT_FOUND_DLL,
    ERROR_NOT_FOUND_DLL_FUNC
};

enum PREDEFINED_CONSTS
{
    PRINT_STRING = 54,
    PRINT_NUMBER,
    PRINT_STACK,

};
enum TYPES
{
    TYPE_BYTE = 1,
    TYPE_WORD,
    TYPE_DWORD,
    TYPE_QWORD,
    TYPE_PTR,
};
std::map<int, std::string> ARG_D;

enum ARG_TYPES
{
    ARG_NULL = 0,
    ARG_NUM,
    ARG_REG,
    ARG_NAME,
    ARG_VAR,
    ARG_VAR_MEMBER,
    ARG_LABEL,
    ARG_LABEL_IT,
    ARG_STR,
    ARG_FUNC,
    ARG_FUNC_IT,
    ARG_FUNC_MEMBER,
    ARG_FUNC_MEMBER_IT,
    ARG_PTR_UNREF,
    ARG_DLL_FUNC,
    ARG_ERROR,
    ARG_UNREF_MASK = 0x80
};

enum ERROR_MODES
{
    MODE_SILENT = 0,
    MODE_PRINTF,
    MODE_MSGBOX,
    MODE_LOG
};

enum FLAG_MODES
{
    FLAG_NOT_SET = 0,
    FLAG_EQUAL,
    FLAG_HIGH,
    FLAG_LOW
};

enum FUNC_RETURN_CODES
{
    RET_SUCCESS = 0,
    RET_ERROR_FATAL,
    RET_ERROR_CONTINUE
};

const int STRING_SIZE = 200;

void FillConstsMap (StrTo64Map_t* map)
{
    #define ADD_CONST(name) (*map)[#name] = name

    ADD_CONST (PRINT_STRING);
    ADD_CONST (PRINT_NUMBER);
    ADD_CONST (PRINT_STACK);
    #undef ADD_CONST
}

#include "Jit_Compiler.h"
#include "HelpFuncs.cpp"

typedef std::map<StrTo32Pair_t, VarDescriptor_t> VarMap_t;
typedef std::map<long long, std::map<std::string, MemberVarDescriptor_t>> MembVarMap_t;

typedef std::map<std::string, FuncStructPair_t> MemberFuncMap_t;

typedef std::pair<std::string, long long> DllPair_t;
typedef std::vector<DllPair_t> DllVector_t;

typedef std::map<std::string, DllVector_t> DllImportMap_t;
typedef StrTo64Map_t                       DllFuncsMap_t;
typedef std::map<long long, void*>         ResolvedDllFuncsMap_t;


#include "Types.h"
#include "Map.cpp"
#include "RegInfo.h"

#endif // CONSTS_H_INCLUDED
