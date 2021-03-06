
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

class VirtualProcessor_t
{
    DISABLE_CLASS_COPY (VirtualProcessor_t)
    void FillJitCompiler (JitCompiler_t* compiler,
                          std::string filename,
                          std::string log);
    void PatchJmp (JitCompiler_t* compiler);
    void JmpPatchRequestLine (int64_t offset, int64_t line);
    void JmpPatchRequestOffset (int64_t offset1, int64_t offset2);
    RunInstanceDataHandler_t* instance_;
    exception_data* expn_;
    int error_mode_;
    FILE* log_;
    std::map<std::string, UserFunc_t>    regFuncs_;
    ErrorReturn_t currentReturn_;
    std::vector<JmpPatchData_t> patch_jmp_;

public:
    static VirtualProcessor_t* currentlyExecuting_;

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

    VirtualProcessor_t (exception_data* expn, int error_mode = MODE_SILENT);

    void RunScript (std::string filename,
                    std::string log = std::string ());

    void RunScriptJit (std::string filename,
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

    friend void OnSigAbort (int);
    friend void OnSigArgSwitch (std::string* error, int8_t* flag, int64_t* arg);
    friend void OnSigEmitInfo (std::string* error);
    friend void OnSigFpe (int);
    friend void OnSigIll (int);
    friend void OnSigInt (int);
    friend void OnSigSegv (int);
    friend void OnSigTerm (int);

    ~VirtualProcessor_t ()
    { }
};

VirtualProcessor_t* VirtualProcessor_t::currentlyExecuting_ = nullptr;

VirtualProcessor_t::VirtualProcessor_t (exception_data* expn, int error_mode) :
    instance_      (nullptr),
    expn_          (expn),
    error_mode_    (error_mode),
    log_           (nullptr),
    regFuncs_      (),
    currentReturn_ (),
    patch_jmp_     ()
{}

void VirtualProcessor_t::RunScript (std::string filename, std::string log)
{
    currentlyExecuting_ = this;
    std::map<int, std::string> checkIfError;
    checkIfError[MODE_LOG] = std::string ("check ") + log;
    checkIfError[MODE_PRINTF] = "check console or STD_OUTPUT_HANDLE redirection";
    checkIfError[MODE_MSGBOX] = "check for Win32 message boxes";
    checkIfError[MODE_SILENT] = "logging disabled";

    #define _PRINTF_TEXT_(funcName) \
                          "%s occurred in \"%s\" on line %I64u\nError text:\n\"%s\"\n", \
                          currentReturn_.retVal == RET_ERROR_FATAL ? "Fatal error" : "Error",\
                          funcName, \
                          instance_->run_line_, \
                          currentReturn_.errorText.c_str ()

    #define CHECK_ERROR(funcName) \
    if (currentReturn_.retVal != RET_SUCCESS) \
    { \
        switch (error_mode_) \
        { \
             default: \
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
        str_error += checkIfError[error_mode_] + ")"; \
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
            //printf ("LINE %I64u\n", instance_->run_line_ + 1);
            //getch ();
            try
            {
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
                        default:
                            break;
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
            }
            catch (ExceptionHandler& ex)
            {
                ex.WriteLog (expn_);

                int res = ErrorPrintfBox (GetForegroundWindow (),
                                          MB_YESNO,
                                          "Error occurred. Check %s\nProceed with execution?",
                                          expn_->filename_.c_str());
                if (res == IDNO)
                {
                    CONS_EXCEPTION (expn_, "Fatal error", ERROR_FATAL, ex)
                }
            }
        }
    }
    CATCH_CONS (instance_->expn_, "Virtual processor crashed", ERROR_VIRTUAL_PROC_CRASHED)

    //instance_->dataStack_.Dump ();

    if (log_)
    {
        fclose (log_);
        log_ = nullptr;
    }

    delete instance_;

    #undef _PRINTF_TEXT_

    #undef HANDLE_CALL

    #undef FuncCase

    #undef CHECK_ERROR

    currentlyExecuting_ = nullptr;
}

void VirtualProcessor_t::RunScriptJit (std::string filename, std::string log)
{
    try
    {
        currentlyExecuting_ = this;
        JitCompiler_t compiler;
        FillJitCompiler (&compiler, filename, log);
        compiler.BuildAndRun ();

        delete instance_;
        currentlyExecuting_ = nullptr;

    }
    CATCH_CONS (instance_->expn_, "Virtual processor crashed", ERROR_VIRTUAL_PROC_CRASHED)
}

void VirtualProcessor_t::PatchJmp (JitCompiler_t* compiler)
{
    std::vector<uint8_t>* mcode = compiler->GetData ();
    //ErrorPrintfBox ("SIZE %d", mcode->size());

    STL_LOOP (it, patch_jmp_)
    {
        int32_t offset = static_cast<int32_t> (0 - (it->first - (it->offset ? (it->second) : (instance_->func_offsets_[it->second]))) - (sizeof (int32_t) + 1));
        //ErrorPrintfBox ("JMP %d", mcode->end() - mcode->begin() - it->first);
        for (size_t i = 0; i < sizeof (offset); i++)
        {
            (*mcode)[it->first + i] = (uint8_t (offset >> i * 8));
        }
    }
    //ErrorPrintfBox ("patch ok");
}

void VirtualProcessor_t::JmpPatchRequestLine (int64_t offset, int64_t line)
{
    patch_jmp_.push_back ((JmpPatchData_t){static_cast<int32_t> (offset),
                                           static_cast<int32_t> (line),
                                           false});
}

void VirtualProcessor_t::JmpPatchRequestOffset (int64_t offset1, int64_t offset2)
{
    patch_jmp_.push_back ((JmpPatchData_t){static_cast<int32_t> (offset1),
                                           static_cast<int32_t> (offset2),
                                           true});
}

void VirtualProcessor_t::FillJitCompiler (JitCompiler_t* compiler, std::string filename, std::string log)
{
    std::map<int, std::string> checkIfError;
    checkIfError[MODE_LOG] = std::string ("check ") + log;
    checkIfError[MODE_PRINTF] = "check console or STD_OUTPUT_HANDLE redirection";
    checkIfError[MODE_MSGBOX] = "check for Win32 message boxes";
    checkIfError[MODE_SILENT] = "logging disabled";

    #define _PRINTF_TEXT_(funcName) \
                          "%s occurred in \"%s\" on line %I64u\nError text:\n\"%s\"\n", \
                          currentReturn_.retVal == RET_ERROR_FATAL ? "Fatal error" : "Error",\
                          funcName, \
                          instance_->run_line_, \
                          currentReturn_.errorText.c_str ()

    #define CHECK_ERROR(funcName) \
    if (currentReturn_.retVal != RET_SUCCESS) \
    { \
        switch (error_mode_) \
        { \
             default: \
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
        str_error += checkIfError[error_mode_] + ")"; \
        NAT_EXCEPTION (instance_->expn_, str_error.c_str (), ERROR_FATAL_RETURN) \
    }

    #define HANDLE_CALL(funcName, expression) \
    {\
        expression; \
        CHECK_ERROR (#funcName) \
    }

    #define FuncCase(name) \
    case CMD_##name: \
        HANDLE_CALL (Jit_##name, Jit_##name (compiler, static_cast<int32_t> (i))); \
        break;


    instance_ = new RunInstanceDataHandler_t (filename, expn_, regFuncs_);

    bool need_realignment = false;
    //int16_t int3 = 0xC3CC;
    //((void(*)())&int3)();
    //Print();
    //((void(*)())&int3)();

    //compiler->int3();
    compiler->push (compiler->r_rbp);
    compiler->mov (compiler->r_rbp, compiler->r_rsp);

    for (size_t i = 0; i < instance_->funcs_.size (); i ++)
    {
        //printf ("Line %d\n", static_cast<int32_t> (i));
        instance_->func_offsets_[i] = static_cast <int64_t> (compiler->Size () + 1);
        //if (i == 26) compiler->int3();

        if (instance_->funcs_[i].flag == CMD_Func)
        {
            if (need_realignment)
            {
                compiler->push<int64_t> (compiler->r_rax);
                compiler->mov<int64_t> (compiler->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
                compiler->mov<int64_t> (&compiler->r_rax, i);
                compiler->pop<int64_t> (compiler->r_rax);
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
                    compiler->push<int64_t> (compiler->r_rax);
                    compiler->mov<int64_t> (compiler->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
                    compiler->inc<int64_t> (&compiler->r_rax);
                    compiler->pop<int64_t> (compiler->r_rax);
                    break;
            }
        }
        else
        if (instance_->funcs_[i].flag == CMD_UFunc)
        {
            compiler->push (reinterpret_cast <int64_t> (instance_->callImportFuncs_[instance_->funcs_[i].cmd].ptr));
            compiler->push (reinterpret_cast <int64_t> (instance_));
            compiler->mov  (compiler->r_rax, reinterpret_cast <int64_t> (instance_->callImportFuncs_[instance_->funcs_[i].cmd].func));
            compiler->call (compiler->r_rax);
            compiler->push<int64_t> (compiler->r_rax);
            compiler->mov<int64_t> (compiler->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
            compiler->inc<int64_t> (&compiler->r_rax);
            compiler->pop<int64_t> (compiler->r_rax);
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
            compiler->push<int64_t> (compiler->r_rax);
            compiler->mov<int64_t> (compiler->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
            compiler->mov<int64_t> (&compiler->r_rax, j + 1);
            compiler->pop<int64_t> (compiler->r_rax);
            compiler->jmp (0);
            JmpPatchRequestLine (compiler->Size() - sizeof (int32_t), j + 1);
        }
        else
            need_realignment = true;

        //printf ("LINE %d\n", i);
    }
    compiler->mov (compiler->r_rsp, compiler->r_rbp);
    compiler->pop (compiler->r_rbp);
    //compiler->int3();
    compiler->retn ();
    PatchJmp (compiler);
}


#include "SignalHandling.cpp"


#include "Functions.h"
