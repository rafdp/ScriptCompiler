
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
public:
    RunInstanceDataHandler_t* instance_;
    exception_data* expn_;
    int error_mode_;
    FILE* log_;
    std::map<std::string, UserFunc_t>    regFuncs_;

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
        regFuncs_.clear();
    }

    VirtualProcessor_t (exception_data* expn);

    void RunScript (std::string filename, int error_mode = MODE_SILENT, std::string log = std::string());

#define FUNC_PRT(name) ErrorReturn_t name  (Arg_t arg);

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
#undef FUNC_PRT
};

VirtualProcessor_t::VirtualProcessor_t(exception_data* expn) :
    instance_   (nullptr),
    expn_       (expn),
    error_mode_ (MODE_SILENT),
    log_        (nullptr),
    regFuncs_   ()
{}

void VirtualProcessor_t::RunScript (std::string filename, int error_mode, std::string log)
{
    std::map<int, std::string> checkIfError;
    checkIfError[MODE_LOG] = std::string ("check ") + log;
    checkIfError[MODE_PRINTF] = "check console or STD_OUTPUT_HANDLE redirection";
    checkIfError[MODE_MSGBOX] = "check for Win32 message boxes";
    checkIfError[MODE_SILENT] = "logging disabled";

    #define _PRINTF_TEXT_(ret, funcName) \
                          "%s occurred in \"%s\" on line %d\nError text:\n\"%s\"\n", \
                          ret.retVal == RET_ERROR_FATAL ? "Fatal error" : "Error",\
                          funcName, \
                          instance_->run_line_, \
                          ret.errorText.c_str()

    #define CHECK_ERROR(res, funcName) \
    if (res.retVal != RET_SUCCESS) \
    { \
        switch (error_mode_) \
        { \
             case MODE_SILENT: \
                break;\
             case MODE_PRINTF: \
                printf (_PRINTF_TEXT_(res, funcName)); \
                break; \
            case MODE_MSGBOX: \
                ErrorPrintfBox (GetForegroundWindow(), 0, _PRINTF_TEXT_(res, funcName)); \
                break; \
            case MODE_LOG: \
                fprintf (log_, _PRINTF_TEXT_(res, funcName)); \
                break; \
        }\
    } \
    if (res.retVal == RET_ERROR_FATAL) \
    { \
        static std::string str_error ("Fatal error occurred ("); \
        str_error += checkIfError[error_mode] + ")"; \
        NAT_EXCEPTION(instance_->expn_, str_error.c_str(), ERROR_FATAL_RETURN) \
    }

    #define HANDLE_CALL(funcName, expression) \
    {\
        ErrorReturn_t result_##funcName = expression; \
        CHECK_ERROR (result_##funcName, #funcName) \
    }

    #define FuncCase(name) \
    case CMD_##name: \
        HANDLE_CALL(name, name (instance_->args_[instance_->run_line_])); \
        break;

    error_mode_ =  error_mode;
    if (error_mode_ == MODE_LOG)
    {
        if (log.empty()) error_mode_ = MODE_SILENT;
        else log_ = fopen (log.c_str(), "w");
    }
    instance_ = new RunInstanceDataHandler_t (filename, expn_,regFuncs_);

    try
    {
        ErrorPrintfBox (GetForegroundWindow(), 0, "About to loop %d %d\n", instance_->run_line_, instance_->funcs_.size());
        for ( ; instance_->run_line_ < instance_->funcs_.size(); instance_->run_line_ ++)
        {
            //printf ("%d ", instance_->run_line_);
            //Sleep (1000);
            if (instance_->funcs_[instance_->run_line_].flag == CMD_Func)
            {
                //printf ("%s\n", CMD_D[instance_->funcs_[instance_->run_line_].cmd].c_str());
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
                }
            }
            else
            if (instance_->funcs_[instance_->run_line_].flag == CMD_UFunc)
            {
                ErrorReturn_t ret = instance_->CallImportFunc(instance_->funcs_[instance_->run_line_].cmd,
                                                              instance_->args_[instance_->run_line_]);
                CHECK_ERROR (ret, instance_->userFuncs_[instance_->funcs_[instance_->run_line_].cmd].c_str())
            }
            else
            if (instance_->funcs_[instance_->run_line_].flag == CMD_CFunc)
            {
                while (instance_->funcs_[instance_->run_line_].cmd != CMD_Ret &&
                       instance_->run_line_ < instance_->funcs_.size()) instance_->run_line_++;
            }
        }
    }
    CATCH_CONS (instance_->expn_, "Virtual processor crashed", ERROR_VIRTUAL_PROC_CRASHED)

    instance_->dataStack_.Dump();

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

#include "Functions.h"
