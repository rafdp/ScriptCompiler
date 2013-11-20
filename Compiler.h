
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include "Stack/StackV2.h"
#include "Map.cpp"
#include "CMD.h"
#include "Argument.h"
#include "ScriptLoader.h"
#include "DataHandling.h"
#include "Consts.h"

class VirtualProcessor_t
{
    void FillJitCompiler (JitCompiler_t* compiler,
                          std::string filename,
                          int error_mode,
                          std::string log);
    void PatchJmp (JitCompiler_t* compiler);
    void JmpPatchRequest (JitCompiler_t* compiler, size_t offset, size_t line);
    RunInstanceDataHandler_t* instance_;
    exception_data* expn_;
    int error_mode_;
    FILE* log_;
    std::map<std::string, UserFunc_t>    regFuncs_;
    ErrorReturn_t currentReturn_;
    std::vector<std::pair<int, int>> patch_jmp_;

public:

    void RegFunc (UserFunc_t f, std::string name)
    {
        regFuncs_[name] = f;
    }

    void RegFuncs (std::pair<UserFunc_t, std::string>* pairs, int size)
    {
        for (int i = 0; i < size; i ++) regFuncs_[pairs[i].second] = pairs[i].first;
    }

    void ClearFuncs ()
    {
        regFuncs_.clear ();
    }

     __declspec(stdcall) void TestFunc() {int i = 0; i++;}

    VirtualProcessor_t (exception_data* expn);

    void RunScript (std::string filename,
                    int error_mode = MODE_SILENT,
                    std::string log = std::string ());

    void RunScriptJit (std::string filename,
                       int error_mode = MODE_SILENT,
                       std::string log = std::string ());

#define FUNC_PRT(name) __attribute__ ((noinline)) void name (); \
                        void Jit_##name (JitCompiler_t* comp, int i);

    FUNC_PRT (RebuildVar)
    FUNC_PRT (Push)
    FUNC_PRT (Pop)
    FUNC_PRT (Mov)
    FUNC_PRT (Print)
    FUNC_PRT (Cmpr)
    FUNC_PRT (Jmp)
    FUNC_PRT (Je)
    FUNC_PRT (Jne)
    FUNC_PRT (Ja)
    FUNC_PRT (Jae)
    FUNC_PRT (Jb)
    FUNC_PRT (Jbe)
    FUNC_PRT (Ret)
    FUNC_PRT (Call)
    FUNC_PRT (Decr)
    FUNC_PRT (InitStackDumpPoint)
    FUNC_PRT (JIT_Printf)
    FUNC_PRT (JIT_Call_Void)
    FUNC_PRT (JIT_Call_DWord)
    FUNC_PRT (JIT_Call_QWord)
    FUNC_PRT (Add)
    FUNC_PRT (Sub)
    FUNC_PRT (Mul)
    FUNC_PRT (Div)
    FUNC_PRT (Adds)
    FUNC_PRT (Subs)
    FUNC_PRT (Muls)
    FUNC_PRT (Divs)
    FUNC_PRT (Lea)
    FUNC_PRT (Sqrt)
#undef FUNC_PRT
};

VirtualProcessor_t::VirtualProcessor_t (exception_data* expn) :
    instance_      (nullptr),
    expn_          (expn),
    error_mode_    (MODE_SILENT),
    log_           (nullptr),
    regFuncs_      (),
    currentReturn_ (),
    patch_jmp_     ()
{}

void VirtualProcessor_t::RunScript (std::string filename, int error_mode, std::string log)
{
    std::map<int, std::string> checkIfError;
    checkIfError[MODE_LOG] = std::string ("check ") + log;
    checkIfError[MODE_PRINTF] = "check console or STD_OUTPUT_HANDLE redirection";
    checkIfError[MODE_MSGBOX] = "check for Win32 message boxes";
    checkIfError[MODE_SILENT] = "logging disabled";

    #define _PRINTF_TEXT_(funcName) \
                          "%s occurred in \"%s\" on line %d\nError text:\n\"%s\"\n", \
                          currentReturn_.retVal == RET_ERROR_FATAL ? "Fatal error" : "Error",\
                          funcName, \
                          instance_->run_line_, \
                          currentReturn_.errorText.c_str ()

    #define CHECK_ERROR(funcName) \
    if (currentReturn_.retVal != RET_SUCCESS) \
    { \
        switch (error_mode_) \
        { \
             case MODE_SILENT: \
                break;\
             case MODE_PRINTF: \
                printf (_PRINTF_TEXT_ (funcName)); \
                break; \
            case MODE_MSGBOX: \
                ErrorPrintfBox (GetForegroundWindow (), 0, _PRINTF_TEXT_ (funcName)); \
                break; \
            case MODE_LOG: \
                fprintf (log_, _PRINTF_TEXT_ (funcName)); \
                break; \
        }\
    } \
    if (currentReturn_.retVal == RET_ERROR_FATAL) \
    { \
        static std::string str_error ("Fatal error occurred ("); \
        str_error += checkIfError[error_mode] + ")"; \
        NAT_EXCEPTION (instance_->expn_, str_error.c_str (), ERROR_FATAL_RETURN) \
    }

    #define HANDLE_CALL(funcName, expression) \
    {\
        expression; \
        CHECK_ERROR (#funcName) \
    }

    #define FuncCase(name) \
    case CMD_##name: \
        HANDLE_CALL (name, name ()); \
        break;

    error_mode_ =  error_mode;
    if (error_mode_ == MODE_LOG)
    {
        if (log.empty ()) error_mode_ = MODE_SILENT;
        else log_ = fopen (log.c_str (), "w");
    }

    instance_ = new RunInstanceDataHandler_t (filename, expn_, regFuncs_);

    try
    {
        //ErrorPrintfBox (GetForegroundWindow (), 0, "About to loop %d %d\n", instance_->run_line_, instance_->funcs_.size ());
        currentReturn_ = ErrorReturn_t (RET_SUCCESS);
        for ( ; instance_->run_line_ < instance_->funcs_.size (); instance_->run_line_ ++)
        {
            //printf ("LINE %d\n", instance_->run_line_);
            if (instance_->funcs_[instance_->run_line_].flag == CMD_Func)
            {
                switch (instance_->funcs_[instance_->run_line_].cmd)
                {
                    FuncCase (RebuildVar)
                    FuncCase (Push)
                    FuncCase (Pop)
                    FuncCase (Mov)
                    FuncCase (Print)
                    FuncCase (Cmpr)
                    FuncCase (Jmp)
                    FuncCase (Je)
                    FuncCase (Jne)
                    FuncCase (Ja)
                    FuncCase (Jae)
                    FuncCase (Jb)
                    FuncCase (Jbe)
                    FuncCase (Ret)
                    FuncCase (Call)
                    FuncCase (Decr)
                    FuncCase (InitStackDumpPoint)
                    FuncCase (JIT_Printf)
                    FuncCase (JIT_Call_Void)
                    FuncCase (JIT_Call_DWord)
                    FuncCase (JIT_Call_QWord)
                    FuncCase (Add)
                    FuncCase (Sub)
                    FuncCase (Mul)
                    FuncCase (Div)
                    FuncCase (Adds)
                    FuncCase (Subs)
                    FuncCase (Muls)
                    FuncCase (Divs)
                    FuncCase (Lea)
                    FuncCase (Sqrt)
                }
            }
            else
            if (instance_->funcs_[instance_->run_line_].flag == CMD_UFunc)
            {
                instance_->CallImportFunc (instance_->funcs_[instance_->run_line_].cmd);
                CHECK_ERROR (instance_->userFuncs_[instance_->funcs_[instance_->run_line_].cmd].c_str ())
            }
            else
            if (instance_->funcs_[instance_->run_line_].flag == CMD_CFunc)
            {
                while (instance_->run_line_ < instance_->funcs_.size ())
                {
                    if (instance_->funcs_[instance_->run_line_].cmd == CMD_Ret) break;
                    //if (instance_->run_line_ > 60) ErrorPrintfBox ("%d", instance_->funcs_[instance_->run_line_].cmd);
                    instance_->run_line_++;
                }
            }

            //printf ("LINE %d\n", instance_->run_line_);
        }
    }
    CATCH_CONS (instance_->expn_, "Virtual processor crashed", ERROR_VIRTUAL_PROC_CRASHED)

    instance_->dataStack_.Dump ();

    if (log_)
    {
        fclose (log_);
        log_ = nullptr;
    }

    delete instance_;

    #undef _PRINTF_TEXT_

    #undef HANDLE_CALL

    #undef FuncCase
}

void VirtualProcessor_t::RunScriptJit (std::string filename, int error_mode, std::string log)
{
    JitCompiler_t compiler;
    FillJitCompiler (&compiler, filename, error_mode, log);
    compiler.BuildAndRun ();

    delete instance_;
}

void VirtualProcessor_t::PatchJmp (JitCompiler_t* compiler)
{
    std::vector<uint8_t>* mcode = compiler->GetData ();

    STL_LOOP (it, patch_jmp_)
    {
        int offset = 0 - (it->first - (int)instance_->func_offsets_[it->second]) - 5;
        for (size_t i = 0; i < sizeof (offset); i++)
        {
            (*mcode)[it->first + i] = (uint8_t (offset >> i * 8));
        }
    }
}

void VirtualProcessor_t::JmpPatchRequest (JitCompiler_t* compiler, size_t offset, size_t line)
{
    patch_jmp_.push_back (std::pair <int, int> (offset, line));
}

void VirtualProcessor_t::FillJitCompiler (JitCompiler_t* compiler, std::string filename, int error_mode, std::string log)
{
    std::map<int, std::string> checkIfError;
    checkIfError[MODE_LOG] = std::string ("check ") + log;
    checkIfError[MODE_PRINTF] = "check console or STD_OUTPUT_HANDLE redirection";
    checkIfError[MODE_MSGBOX] = "check for Win32 message boxes";
    checkIfError[MODE_SILENT] = "logging disabled";

    #define _PRINTF_TEXT_(funcName) \
                          "%s occurred in \"%s\" on line %d\nError text:\n\"%s\"\n", \
                          currentReturn_.retVal == RET_ERROR_FATAL ? "Fatal error" : "Error",\
                          funcName, \
                          instance_->run_line_, \
                          currentReturn_.errorText.c_str ()

    #define CHECK_ERROR(funcName) \
    if (currentReturn_.retVal != RET_SUCCESS) \
    { \
        switch (error_mode_) \
        { \
             case MODE_SILENT: \
                break;\
             case MODE_PRINTF: \
                printf (_PRINTF_TEXT_ (funcName)); \
                break; \
            case MODE_MSGBOX: \
                ErrorPrintfBox (GetForegroundWindow (), 0, _PRINTF_TEXT_ (funcName)); \
                break; \
            case MODE_LOG: \
                fprintf (log_, _PRINTF_TEXT_ (funcName)); \
                break; \
        }\
    } \
    if (currentReturn_.retVal == RET_ERROR_FATAL) \
    { \
        static std::string str_error ("Fatal error occurred ("); \
        str_error += checkIfError[error_mode] + ")"; \
        NAT_EXCEPTION (instance_->expn_, str_error.c_str (), ERROR_FATAL_RETURN) \
    }

    #define HANDLE_CALL(funcName, expression) \
    {\
        expression; \
        CHECK_ERROR (#funcName) \
    }

    #define FuncCase(name) \
    case CMD_##name: \
        HANDLE_CALL (Jit_##name, Jit_##name (compiler, i)); \
        break;


    instance_ = new RunInstanceDataHandler_t (filename, expn_, regFuncs_);

    bool need_realignment = false;
    //short int3 = 0xC3CC;
    //((void(*)())&int3)();
    //Print();
    //((void(*)())&int3)();

    compiler->int3();
    compiler->push (compiler->r_rbp);
    compiler->mov (compiler->r_rbp, compiler->r_rsp);

    try
    {
        for (size_t i = 0; i < instance_->funcs_.size (); i ++)
        {
            instance_->func_offsets_[i] = compiler->Size () + 1;

            if (instance_->funcs_[i].flag == CMD_Func)
            {
                if (need_realignment)
                {
                    compiler->mov (&instance_->run_line_, i);
                    need_realignment = false;
                }
                switch (instance_->funcs_[i].cmd)
                {
                    FuncCase (RebuildVar)
                    FuncCase (Push)
                    FuncCase (Pop)
                    FuncCase (Mov)
                    FuncCase (Print)
                    FuncCase (Cmpr)
                    FuncCase (Jmp)
                    FuncCase (Je)
                    FuncCase (Jne)
                    FuncCase (Ja)
                    FuncCase (Jae)
                    FuncCase (Jb)
                    FuncCase (Jbe)
                    FuncCase (Ret)
                    FuncCase (Call)
                    FuncCase (Decr)
                    FuncCase (InitStackDumpPoint)
                    FuncCase (JIT_Printf)
                    FuncCase (JIT_Call_Void)
                    FuncCase (JIT_Call_DWord)
                    FuncCase (JIT_Call_QWord)
                    FuncCase (Add)
                    FuncCase (Sub)
                    FuncCase (Mul)
                    FuncCase (Div)
                    FuncCase (Adds)
                    FuncCase (Subs)
                    FuncCase (Muls)
                    FuncCase (Divs)
                    FuncCase (Lea)
                    FuncCase (Sqrt)
                    default:
                        compiler->inc  (&instance_->run_line_);
                        break;
                }
            }
            else
            if (instance_->funcs_[i].flag == CMD_UFunc)
            {
                compiler->push (instance_->callImportFuncs_[instance_->funcs_[i].cmd].ptr);
                compiler->push (instance_);
                compiler->mov  (compiler->r_rax, instance_->callImportFuncs_[instance_->funcs_[i].cmd].func);
                compiler->call (compiler->r_rax);
                compiler->inc  (&instance_->run_line_);
            }
            else
            if (instance_->funcs_[i].flag == CMD_CFunc)
            {
                size_t j = i;
                while (j < instance_->funcs_.size ())
                {
                    if (instance_->funcs_[j].cmd == CMD_Ret) break;
                    j++;
                }
                compiler->mov  (&instance_->run_line_, j + 1);
                patch_jmp_.push_back (std::pair <int, int> (compiler->Size() + 1, j + 1));
                compiler->jmp (0);
            }
            else
                need_realignment = true;
                //compiler->inc  (&instance_->run_line_);

            //printf ("LINE %d\n", i);
        }
    }
    CATCH_CONS (instance_->expn_, "Virtual processor crashed", ERROR_VIRTUAL_PROC_CRASHED)
    compiler->mov (compiler->r_rsp, compiler->r_rbp);
    compiler->pop (compiler->r_rbp);
    compiler->retn ();

    PatchJmp (compiler);
}

#include "Functions.h"
