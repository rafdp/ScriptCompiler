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
const int VERSION = 2007;

const char CONTROL_CHARACTER = 4;

std::map<std::string, int> CMD_A;
std::map<std::string, int> CMD_A_service;
std::map<int, std::string> CMD_D;
/**
enum Cmd_Assembler
{
    CMD_PUSH     = 100,    //ok
    CMD_POP      = 101,    //ok
    CMD_DUP      = 102,    //ok
    CMD_MULS     = 103,    //ok
    CMD_ADDS     = 104,    //ok
    CMD_SUBS     = 105,    //ok
    CMD_DIVS     = 106,    //ok

    CMD_JMP      = 200,    //ok
    CMD_JE       = 201,    //ok
    CMD_JNE      = 202,    //ok
    CMD_JA       = 203,    //ok
    CMD_JL       = 204,    //ok
    CMD_JAE      = 205,    //ok
    CMD_JLE      = 206,    //ok

    CMD_SEL      = 300,    //ok
    CMD_SET      = 301,    //ok
    CMD_CMPR     = 302,    //ok
    CMD_MULR     = 303,    //ok
    CMD_ADDR     = 304,    //ok
    CMD_SUBR     = 305,    //ok
    CMD_DIVR     = 306,    //ok
    CMD_RESULT   = 307,    //ok
    //CMD_Func     = 308,    //ok
    CMD_CALL     = 309,    //ok
    CMD_RET      = 310,    //ok
    CMD_START    = 311,    //ok
    //CMD_EXT      = 312,    //ok
    //CMD_EXT_F    = 313,    //ok
    CMD_INPUT    = 314,    //ok
    CMD_INPUT_   = 315,    //ok
    CMD_S_INPUT  = 316,    //ok
    CMD_S_INPUT_ = 317,    //ok

    CMD_ASCII    = 400,    //ok
    CMD_PRINTR   = 401,    //ok
    CMD_PRINTS   = 402,    //ok
    CMD_PRINTL   = 403,    //ok
    CMD_EOF      = 404,    //ok
    //CMD_Var      = 405,    //ok
    CMD_DEL      = 406,    //ok
    CMD_GETCH    = 407,    //ok
    CMD_SLEEP    = 408,    //ok
    CMD_Link     = 409     //ok
};
void SetMapAssembler()
{
    static bool set = false;
    if (set)
    {
        return;
    }

    CMD_A["push"]     = CMD_PUSH;
    CMD_A["pop"]      = CMD_POP;
    CMD_A["dup"]      = CMD_DUP;
    CMD_A["muls"]     = CMD_MULS;
    CMD_A["adds"]     = CMD_ADDS;
    CMD_A["subs"]     = CMD_SUBS;
    CMD_A["divs"]     = CMD_DIVS;

    CMD_A["jmp"]      = CMD_JMP;
    CMD_A["je"]       = CMD_JE;
    CMD_A["jne"]      = CMD_JNE;
    CMD_A["ja"]       = CMD_JA;
    CMD_A["jl"]       = CMD_JL;
    CMD_A["jae"]      = CMD_JAE;
    CMD_A["jle"]      = CMD_JLE;

    CMD_A["sel"]      = CMD_SEL;
    CMD_A["set"]      = CMD_SET;
    CMD_A["cmpr"]     = CMD_CMPR;
    CMD_A["mulr"]     = CMD_MULR;
    CMD_A["addr"]     = CMD_ADDR;
    CMD_A["subr"]     = CMD_SUBR;
    CMD_A["divr"]     = CMD_DIVR;
    CMD_A["result"]   = CMD_RESULT;
    //CMD_A["func"]     = CMD_Func;
    CMD_A["call"]     = CMD_CALL;
    CMD_A["ret"]      = CMD_RET;
    CMD_A["start"]    = CMD_START;
    //CMD_A["extern"]   = CMD_EXT;
    CMD_A["input"]    = CMD_INPUT;
    CMD_A["input_"]   = CMD_INPUT_;
    CMD_A["s_input"]  = CMD_S_INPUT;
    CMD_A["s_input_"] = CMD_S_INPUT_;

    CMD_A["ascii"]    = CMD_ASCII;
    CMD_A["printr"]   = CMD_PRINTR;
    CMD_A["prints"]   = CMD_PRINTS;
    CMD_A["printl"]   = CMD_PRINTL;
    CMD_A["eof"]      = CMD_EOF;
    //CMD_A["var"]      = CMD_Var;
    CMD_A["del"]      = CMD_DEL;
    CMD_A["getch"]    = CMD_GETCH;
    CMD_A["sleep"]    = CMD_SLEEP;
    CMD_A["link"]     = CMD_Link;
    set = true;
}
*/


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
    //CMD_Pointer,
    //! service

    CMD_Push     = 100,
    CMD_Pop,
    CMD_Mov,
    CMD_Print,
    CMD_Cmpr,
    CMD_Jmp,
    CMD_Je,
    CMD_Jne,
    CMD_Ja,
    CMD_Jb,
    CMD_Jae,
    CMD_Jbe,
    CMD_Call,
    CMD_Ret,
    CMD_Decr,
    CMD_InitStackDumpPoint,
    CMD_JIT_Printf,
    CMD_JIT_Call_Void,
    CMD_JIT_Call_DWord,
    CMD_JIT_Call_QWord,
    CMD_Add,
    CMD_Sub,
    CMD_Mul,
    CMD_Div,
    CMD_Adds,
    CMD_Subs,
    CMD_Muls,
    CMD_Divs,
    CMD_Lea
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
    CMD_A_service["import"]       = CMD_Import;
    CMD_A_service["var"]        = CMD_Var;
    CMD_A_service["link"]       = CMD_Link;
    CMD_A_service["struct"]     = CMD_Struct;
    CMD_A_service["struct_end"] = CMD_End_Struct;
    CMD_A_service["plabel"]     = CMD_PLabel;
    CMD_A_service["pfunc"]      = CMD_PFunc;
    //CMD_A_service["pointer"]    = CMD_Pointer;

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

    ARG_D[ARG_NULL]   = "none";
    ARG_D[ARG_NUM]    = "num";
    ARG_D[ARG_REG]    = "reg";
    ARG_D[ARG_VAR]    = "var";
    ARG_D[ARG_LABEL]  = "label";
    ARG_D[ARG_STR]    = "str";
    ARG_D[ARG_FUNC]   = "func";
    ARG_D[ARG_ERROR]  = "error";
    ARG_D[ARG_VAR_MEMBER]  = "var_member";
    ARG_D[ARG_DLL_FUNC]  = "dll_func";
    //ARG_D[ARG_PTR]  = "pointer";

    set = true;
}


std::map<std::string, int> CMD_REG;

enum Registers
{
    CMD_BADINP = -1,

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
    REG_PTQWORD = 0x63,
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

        /*if (Different ("Map.cpp", "MapBak.cpp"))
        {
            Backup ("Map.cpp", "MapBak.cpp");
            FILE* bak = fopen ("MapBak.cpp", "r");
            int build = 0;
            char c = 0;
            while ((c = fgetc (bak)) != '2');
            while ((c = fgetc (bak)) != ';') {build *= 10; build += c - '0';}
            build++;
            FILE* map = fopen ("Map_.cpp", "w");

            fprintf (map, "const int VERSION = 2%05d;", build);

            fseek (bak, SEEK_SET, 27);
            while (!feof (bak))
            {
                char c = fgetc (bak);
                fwrite (&c, 1, 1, map);
            }
            fclose (map);
            fclose (bak);
            Backup ("Map_.cpp", "MapBak.cpp");
        }
        */

    }
};
AutoMapValuesInitialisator_t AutoMapValuesInitialisator;

#endif
