#ifndef CONSTS_H_INCLUDED
#define CONSTS_H_INCLUDED

//#define nullptr NULL


#define STL_LOOP(iterator, object) for (auto iterator = (object).begin (); iterator != (object).end (); iterator++)
#define STL_RLOOP(iterator, object) for (auto iterator = (object).begin (); iterator != (object).end (); iterator++)

typedef int64_t QWORD;
typedef std::map<std::string, long long> StrTo64Map_t;
typedef std::pair<std::string, int> StrTo32Pair_t;
const int MAX_BUFFER = 4096;

int ErrorPrintfBox (HWND wnd, DWORD flags, const char* format, ...)
{
    if (!format) return 0;

    char str[MAX_BUFFER] = "";

    va_list arg; va_start (arg, format);
    int n = _vsnprintf (str, sizeof (str) - 1, format, arg);
    va_end (arg);

    MessageBoxA (wnd, str, "", flags);
    return n;
}

int ErrorPrintfBox (const char* format, ...)
{
    if (!format) return 0;

    char str[MAX_BUFFER] = "";

    va_list arg; va_start (arg, format);
    int n = _vsnprintf (str, sizeof (str) - 1, format, arg);
    va_end (arg);

    MessageBoxA (GetForegroundWindow (), str, "", 0);
    return n;
}


enum ERRORS
{
    ERROR_SCRIPT_OPEN_FILE_CTOR = 5,
    ERROR_SCRIPT_OPEN_FILE_SAVE,         //! 6
    ERROR_CONFLICTING_NAMES,             //! 7
    ERROR_INVALID_HEADER,                //! 8
    ERROR_VIRTUAL_PROC_CRASHED,          //! 9
    ERROR_FATAL_RETURN,                  //! 10
    ERROR_NO_FUNC_IMPORT,                //! 11
    ERROR_INVALID_VALUE_0,               //! 12
    ERROR_NULL_SIZE_STRUCT,              //! 13
    ERROR_MEMBER_NOT_FOUND,              //! 14
    ERROR_ADDING_VAR_STRUCT,             //! 15
    ERROR_INVALID_STRUCT_VAR_NUM_STRUCT, //! 16
    ERROR_PARSER_INVALID_ARGS,           //! 17
    ERROR_NOT_RECOGNIZED_CMD,            //! 18
    ERROR_VAR_ALREADY_EXISTS,            //! 19
    ERROR_PARSER_CRASHED,                //! 20
    ERROR_NESTED_FUNC,                   //! 21
    ERROR_NESTED_STRUCT,                 //! 22
    ERROR_STRUCT_UNEXPECTED_CMD,         //! 23
    ERROR_LINK_UNEXPECTED_CMD,           //! 24
    ERROR_UNDEFINED_REFERENCE_FUNC,      //! 25
    ERROR_UNDEFINED_REFERENCE_LABEL,     //! 26
    ERROR_INVALID_UNREF,                 //! 27
    ERROR_IMPORT_DLL_INVALID_STR,        //! 28
    ERROR_NOT_FOUND_DLL,                 //! 29
    ERROR_NOT_FOUND_DLL_FUNC             //! 30
};

enum PREDEFINED_CONSTS
{
    PRINT_STRING    = 54,
    PRINT_NUMBER, //! 55
    PRINT_STACK,  //! 56

};
enum TYPES
{
    TYPE_BYTE     = 1,
    TYPE_WORD     = 2,
    TYPE_DWORD    = 3,
    TYPE_QWORD    = 4,
    TYPE_PTR      = 5
};
std::map<int, std::string> ARG_D;

enum ARG_TYPES
{
    ARG_NULL              = 0,
    ARG_NUM,            //! 1
    ARG_REG,            //! 2
    ARG_NAME,           //! 3
    ARG_VAR,            //! 4
    ARG_VAR_MEMBER,     //! 5
    ARG_LABEL,          //! 6
    ARG_LABEL_IT,       //! 7
    ARG_STR,            //! 8
    ARG_FUNC,           //! 9
    ARG_FUNC_IT,        //! 10
    ARG_FUNC_MEMBER,    //! 11
    ARG_FUNC_MEMBER_IT, //! 12
    ARG_PTR_UNREF,      //! 13
    ARG_DLL_FUNC,       //! 14
    ARG_ERROR,          //! 15
    ARG_UNREF_MASK = 0x80
};

enum ERROR_MODES
{
    MODE_SILENT    = 0,
    MODE_PRINTF, //! 1
    MODE_MSGBOX, //! 2
    MODE_LOG     //! 3
};

enum FLAG_MODES
{
    FLAG_NOT_SET  = 2,
    FLAG_EQUAL = 0,
    FLAG_HIGH = 1,
    FLAG_LOW = -1
};

enum FUNC_RETURN_CODES
{
    RET_SUCCESS          = 0,
    RET_ERROR_FATAL,   //! 1
    RET_ERROR_CONTINUE //! 2
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

typedef std::map<StrTo32Pair_t, VarDescriptor_t>  VarMap_t;
typedef std::map<long long,
                 std::map<std::string,
                          MemberVarDescriptor_t>> MembVarMap_t;

typedef std::map<std::string, FuncStructPair_t>   MemberFuncMap_t;

typedef std::pair<std::string, long long>         DllPair_t;
typedef std::vector<DllPair_t>                    DllVector_t;

typedef std::map<std::string, DllVector_t>        DllImportMap_t;
typedef StrTo64Map_t                              DllFuncsMap_t;
typedef std::map<long long, void*>                ResolvedDllFuncsMap_t;


#include "Types.h"
#include "Map.cpp"
#include "RegInfo.h"

#endif // CONSTS_H_INCLUDED
