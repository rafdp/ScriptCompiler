#ifndef MAP_CPP_INCLUDED
#define MAP_CPP_INCLUDED

struct MapHeader
{
    int key;
    int version;
    bool operator != (const MapHeader& that)
    {
        if (that.key != key || that.version != version)
            return true;
        return false;
    }
};

const int KEY = 0x7C5C2F7C;
const int VERSION = 2015;

const char CONTROL_CHARACTER = 4;

std::map<std::string, int> CMD_A;
std::map<std::string, int> CMD_A_service;
std::map<int, std::string> CMD_D;

enum Cmd_Assembler
{
    //! service
    CMD_Null   = 1,
    CMD_Func,
    CMD_Label,
    CMD_Const,
    CMD_Name,
    CMD_Extern,
    CMD_UFunc,
    CMD_CFunc,
    CMD_Import,
    CMD_Var,
    CMD_Link,
    CMD_Error,
    CMD_Struct,
    CMD_End_Struct,
    CMD_PLabel,
    CMD_PFunc,
    CMD_RebuildVar,
    //! service

    CMD_Push                  = 100,
    CMD_Pop,                //! 101
    CMD_Mov,                //! 102
    CMD_Print,              //! 103
    CMD_Cmpr,               //! 104
    CMD_Jmp,                //! 105
    CMD_Je,                 //! 106
    CMD_Jne,                //! 107
    CMD_Ja,                 //! 108
    CMD_Jb,                 //! 109
    CMD_Jae,                //! 110
    CMD_Jbe,                //! 111
    CMD_Call,               //! 112
    CMD_Ret,                //! 113
    CMD_Decr,               //! 114
    CMD_InitStackDumpPoint, //! 115
    CMD_JIT_Printf,         //! 116
    CMD_JIT_Call_Void,      //! 117
    CMD_JIT_Call_DWord,     //! 118
    CMD_JIT_Call_QWord,     //! 119
    CMD_Add,                //! 120
    CMD_Sub,                //! 121
    CMD_Mul,                //! 122
    CMD_Div,                //! 123
    CMD_Adds,               //! 124
    CMD_Subs,               //! 125
    CMD_Muls,               //! 126
    CMD_Divs,               //! 127
    CMD_Lea                 //! 128
};
void SetMapAssembler()
{
    static bool set = false;
    if (set)
    {
        return;
    }

    CMD_A_service["const"]      = CMD_Const;
    CMD_A_service["extern"]     = CMD_Extern;
    CMD_A_service["func"]       = CMD_CFunc;
    CMD_A_service["import"]     = CMD_Import;
    CMD_A_service["var"]        = CMD_Var;
    CMD_A_service["link"]       = CMD_Link;
    CMD_A_service["struct"]     = CMD_Struct;
    CMD_A_service["struct_end"] = CMD_End_Struct;
    CMD_A_service["plabel"]     = CMD_PLabel;
    CMD_A_service["pfunc"]      = CMD_PFunc;

    CMD_A["push"]                  = CMD_Push;
    CMD_A["pop"]                   = CMD_Pop;
    CMD_A["mov"]                   = CMD_Mov;
    CMD_A["print"]                 = CMD_Print;
    CMD_A["cmpr"]                  = CMD_Cmpr;
    CMD_A["jmp"]                   = CMD_Jmp;
    CMD_A["je"]                    = CMD_Je;
    CMD_A["jne"]                   = CMD_Jne;
    CMD_A["ja"]                    = CMD_Ja;
    CMD_A["jb"]                    = CMD_Jb;
    CMD_A["jae"]                   = CMD_Jae;
    CMD_A["jbe"]                   = CMD_Jbe;
    CMD_A["ret"]                   = CMD_Ret;
    CMD_A["call"]                  = CMD_Call;
    CMD_A["decr"]                  = CMD_Decr;
    CMD_A["init_stack_dump_point"] = CMD_InitStackDumpPoint;
    CMD_A["jit_printf"]            = CMD_JIT_Printf;
    CMD_A["jit_call_void"]         = CMD_JIT_Call_Void;
    CMD_A["jit_call_dword"]        = CMD_JIT_Call_DWord;
    CMD_A["jit_call_qword"]        = CMD_JIT_Call_QWord;
    CMD_A["add"]                   = CMD_Add;
    CMD_A["sub"]                   = CMD_Sub;
    CMD_A["mul"]                   = CMD_Mul;
    CMD_A["div"]                   = CMD_Div;
    CMD_A["adds"]                  = CMD_Adds;
    CMD_A["subs"]                  = CMD_Subs;
    CMD_A["muls"]                  = CMD_Muls;
    CMD_A["divs"]                  = CMD_Divs;
    CMD_A["lea"]                   = CMD_Lea;

    STL_LOOP (i, CMD_A) CMD_D[i->second] = i->first;

    ARG_D[ARG_NULL]       = "none";
    ARG_D[ARG_NUM]        = "num";
    ARG_D[ARG_REG]        = "reg";
    ARG_D[ARG_VAR]        = "var";
    ARG_D[ARG_LABEL]      = "label";
    ARG_D[ARG_STR]        = "str";
    ARG_D[ARG_FUNC]       = "func";
    ARG_D[ARG_ERROR]      = "error";
    ARG_D[ARG_VAR_MEMBER] = "var_member";
    ARG_D[ARG_DLL_FUNC]   = "dll_func";

    set = true;
}


std::map<std::string, int> CMD_REG;

enum Registers
{
    CMD_BADINP  = -1,

    REG_ABX     = 0x0,
    REG_BBX     = 0x1,
    REG_CBX     = 0x2,
    REG_DBX     = 0x3,
    REG_RB      = 0x4,
    REG_AWX     = 0x5,
    REG_BWX     = 0x7,
    REG_CWX     = 0x9,
    REG_DWX     = 0xB,
    REG_RW      = 0xD,
    REG_ADX     = 0xF,
    REG_BDX     = 0x13,
    REG_CDX     = 0x17,
    REG_DDX     = 0x1B,
    REG_RD      = 0x2F,
    REG_AQX     = 0x23,
    REG_BQX     = 0x2B,
    REG_CQX     = 0x33,
    REG_DQX     = 0x3B,
    REG_RQ      = 0x43,
    REG_PTBYTE  = 0x4B,
    REG_PTWORD  = 0x53,
    REG_PTDWORD = 0x5B,
    REG_PTQWORD = 0x63
};
void SetMapReg()
{
    static bool set = false;
    if (set)
    {
        return;
    }

    CMD_REG["abx"]       = REG_ABX;
    CMD_REG["bbx"]       = REG_BBX;
    CMD_REG["cbx"]       = REG_CBX;
    CMD_REG["dbx"]       = REG_DBX;
    CMD_REG["rbyte"]     = REG_RB;
    CMD_REG["awx"]       = REG_AWX;
    CMD_REG["bwx"]       = REG_BWX;
    CMD_REG["cwx"]       = REG_CWX;
    CMD_REG["dwx"]       = REG_DWX;
    CMD_REG["rword"]     = REG_RW;
    CMD_REG["adx"]       = REG_ADX;
    CMD_REG["bdx"]       = REG_BDX;
    CMD_REG["cdx"]       = REG_CDX;
    CMD_REG["ddx"]       = REG_DDX;
    CMD_REG["rdword"]    = REG_RD;
    CMD_REG["aqx"]       = REG_AQX;
    CMD_REG["bqx"]       = REG_BQX;
    CMD_REG["cqx"]       = REG_CQX;
    CMD_REG["dqx"]       = REG_DQX;
    CMD_REG["rqword"]    = REG_RQ;
    CMD_REG["rptrbyte"]  = REG_PTBYTE;
    CMD_REG["rptrword"]  = REG_PTWORD;
    CMD_REG["rptrdword"] = REG_PTDWORD;
    CMD_REG["rptrqword"] = REG_PTQWORD;

    set = true;
}

const int REGISTERS_SIZE = 5 * (1 + 2 + 4 + 8) + 4 * 8;

class AutoMapValuesInitialisator_t
{
    public:
    AutoMapValuesInitialisator_t ()
    {
        SetMapAssembler();
        SetMapReg();
    }
};
AutoMapValuesInitialisator_t AutoMapValuesInitialisator;

#endif
