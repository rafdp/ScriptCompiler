#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <math.h>
#define nullptr NULL
#include "Stack/StackV2.h"
#include "Consts.h"
#include "ScriptLine.h"
#include "CMD.h"
#include "Argument.h"

class ScriptCompiler_t
{
    void _in_clsf_arg (char* flag,
                       long long* arg,
                       bool error,
                       int line,
                       bool var = true,
                       bool struct_ = false);
    int func_level_;
    int struct_func_level_;
    public:
    std::string                      file_;
    StrTo64Map_t                     consts_;
    std::map<long long, size_t>      typeSizes_;
    VarMap_t                         vars_;
    MembVarMap_t                     mvars_;
    StrTo64Map_t                     userFuncs_;
    StrTo64Map_t                     createdFuncs_;
    MemberFuncMap_t                  memberFuncs_;
    std::map<std::string,
             long long>              labels_;
    std::vector<std::string>         strings_;
    std::vector<Cmd_t>               funcs_;
    std::vector<Arg_t>               args_;
    exception_data*                  expn_;
    std::vector<VarMap_t::iterator>  die_requests_;
    RegisteredTypes_t                regTypes_;
    std::string                      currFunc_;
    long long                        currStruct_;
    bool                             structFunc_;
    DllImportMap_t                   dllImportMap_;
    DllFuncsMap_t                    dllFuncsMap_;

    ScriptCompiler_t (std::string filename,
                      exception_data* expn);
    void PushData (const Cmd_t& cmd,
                   const Arg_t& arg);
    void Save ();

    void ClassifyArg (Arg_t* arg,
                      bool error,
                      int line,
                      bool var = true,
                      bool struct_ = false);

    void Dump ();

    void CreateVarDieRequest (VarMap_t::iterator var)
    {
        die_requests_.push_back (var);
    }

    void ActivateDieRequests (int line)
    {
        STL_LOOP (i, die_requests_)
        {
            (*i)->second.die = line;
        }
        die_requests_.clear ();
    }

    bool AddName (std::string name,
                  char flag,
                  long long cmd,
                  int line);
    bool CheckName (std::string name,
                    int line);

    void ParseLinkFile (std::string file);

    void ManageVars (Cmd_t& cmd,
                    Arg_t& arg,
                    int n_line);

    bool ManageStruct (Cmd_t& cmd,
                       Arg_t& arg,
                       int line);

    void ResolvePrototypes ();

    #define FUNC_PROT(name) void name (Cmd_t& cmd, Arg_t& arg, int n_line);

    FUNC_PROT (Null)
    FUNC_PROT (Func)
    FUNC_PROT (PFunc)
    FUNC_PROT (Struct)
    FUNC_PROT (Link)
    FUNC_PROT (CFunc)
    FUNC_PROT (Label)
    FUNC_PROT (PLabel)
    FUNC_PROT (Const)
    FUNC_PROT (Extern)
    FUNC_PROT (Import)
    FUNC_PROT (Var)
    FUNC_PROT (Name)

    #undef FUNC_PROT

    #define FUNC_PROT_S(name) bool Struct##name (Cmd_t& cmd, Arg_t& arg, int n_line);

    FUNC_PROT_S (Var)
    FUNC_PROT_S (CFunc)
    FUNC_PROT_S (Func)
    FUNC_PROT_S (PFunc)
    FUNC_PROT_S (Label)
    FUNC_PROT_S (PLabel)
    FUNC_PROT_S (Extern)
    FUNC_PROT_S (Name)
    FUNC_PROT_S (Struct)
    FUNC_PROT_S (End_Struct)
    FUNC_PROT_S (Null)

    #undef FUNC_PROT_S

    #define FUNC_PROT_L(name) void Link##name (Cmd_t& cmd, Arg_t& arg);

    FUNC_PROT_L (Null)
    FUNC_PROT_L (Link)
    FUNC_PROT_L (Const)
    FUNC_PROT_L (Extern)

    #undef FUNC_PROT_L
};


#include "BuilderCases.h"

ScriptCompiler_t::ScriptCompiler_t (std::string filename, exception_data* expn) :
    func_level_        (),
    struct_func_level_ (),
    file_              (filename),
    consts_            (),
    typeSizes_         (),
    vars_              (),
    mvars_             (),
    userFuncs_         (),
    createdFuncs_      (),
    memberFuncs_       (),
    labels_            (),
    funcs_             (),
    args_              (),
    expn_              (expn),
    die_requests_      (),
    regTypes_          (&consts_,
                        &typeSizes_,
                        &mvars_,
                        &vars_,
                        &func_level_,
                        &memberFuncs_,
                        expn_),
    currFunc_           (),
    currStruct_         (),
    structFunc_         (false),
    dllImportMap_       (),
    dllFuncsMap_        ()
{
    FillConstsMap (&consts_);

    FILE* f = fopen (filename.c_str (), "r");

    #define CMD_CASE(name) case CMD_##name: name (cmd, arg, n_line); break;

    int n_line = 0;
    try
    {
        for ( ; !feof (f); n_line ++)
        {
            //printf ("LINE %d ", n_line);
            ScriptLine_t line (f);
            Cmd_t cmd (line.cmd);
            Arg_t arg (line.param1, line.param2);

            //printf ("%s %s %s\n", line.cmd.c_str (), line.param1.c_str (), line.param2.c_str ());
            if (line.cmd[0] == ';')
            {
                cmd.Clear ();
                arg.Clear ();
            }
            if (currStruct_ && ManageStruct (cmd, arg, n_line));
            else
            {
                switch (cmd.flag)
                {
                    CMD_CASE (Null)
                    CMD_CASE (Func)
                    CMD_CASE (PFunc)
                    CMD_CASE (Struct)
                    CMD_CASE (Link)
                    CMD_CASE (CFunc)
                    CMD_CASE (Label)
                    CMD_CASE (PLabel)
                    CMD_CASE (Const)
                    CMD_CASE (Extern)
                    CMD_CASE (Import)
                    CMD_CASE (Var)
                    CMD_CASE (Name)
                }
            }
            PushData (cmd, arg);
        }
        ResolvePrototypes ();
    }
    CATCH_CONS (expn_, "Parser crashed", ERROR_PARSER_CRASHED)
    fclose (f);

    #undef CMD_CASE
}

void ScriptCompiler_t::PushData (const Cmd_t& cmd, const Arg_t& arg)
{
    funcs_.push_back (cmd);
    args_.push_back (arg);
}

void ScriptCompiler_t::_in_clsf_arg (char* flag, long long* arg, bool error, int line, bool var, bool struct_)
{
    if ( (*flag & ARG_NAME) == ARG_NAME)
    {
        auto result = consts_.end ();
        auto varResult = vars_.end ();
        auto labResult = labels_.end ();
        #define INVALID_UNREF_CHECK \
            if (*flag & ARG_UNREF_MASK) NAT_EXCEPTION (expn_, "Invalid use of '*'", ERROR_INVALID_UNREF)
        if (var)
        for (int varLevel = (!struct_ ? func_level_ : struct_func_level_); varLevel >= 0 && varResult == vars_.end (); varLevel--)
        {
            varResult = vars_.find (StrTo32Pair_t (*(std::string*) (*arg), varLevel));
            if (varResult != vars_.end () &&
                varResult->second.die != -1 &&
                varResult->second.die < line)
                varResult = vars_.end ();
        }
        //!  operator = returns reference to object
        //!  c = b = a;
        if ( (result = consts_.find (*(std::string*) (*arg))) != consts_.end ())
        {
            INVALID_UNREF_CHECK
            *flag = ARG_NUM;
            delete[] (char*) (*arg);
            *arg = result->second;
        }
        else
        if (var && varResult != vars_.end ())
        {
            if (*flag & ARG_UNREF_MASK)
            {
                if (varResult->second.typeCode == TYPE_PTR) *flag = ARG_VAR | ARG_UNREF_MASK;
                else INVALID_UNREF_CHECK
            }
            else *flag = ARG_VAR;
            delete (std::string*) (*arg);
            *arg = varResult->second.num;
        }
        else
        if ( (labResult = labels_.find ( ( (std::string*) (*arg))->c_str ())) != labels_.end ())
        {
            INVALID_UNREF_CHECK
            if (labResult->second == -1)
            {
                *flag = ARG_LABEL_IT;
            }
            else
            {
                *flag = ARG_LABEL;
                delete (std::string*) (*arg);
                *arg = labResult->second;
            }
        }
        else
        if (IsString ( (std::string*) (*arg)))
        {
            INVALID_UNREF_CHECK
            *flag = ARG_STR;
            ( (std::string*) (*arg))->erase ( ( (std::string*) (*arg))->begin ());
            ( (std::string*) (*arg))->erase ( ( (std::string*) (*arg))->end () - 1);
            strings_.push_back (*(std::string*) (*arg));
            delete (std::string*) (*arg);
            *arg = strings_.size () - 1;
        }
        else
        if ( (result = createdFuncs_.find (*(std::string*) (*arg))) != createdFuncs_.end ())
        {
            INVALID_UNREF_CHECK
            if (result->second == -1)
            {
                *flag = ARG_FUNC_IT;
            }
            else
            {
                *flag = ARG_FUNC;
                delete (std::string*) (*arg);
                *arg = result->second;
            }
        }
        else
        if ( (result = dllFuncsMap_.find (*(std::string*) (*arg))) != dllFuncsMap_.end ())
        {
            INVALID_UNREF_CHECK
            *flag = ARG_DLL_FUNC;
            *arg = result->second;
        }
        else
        if (regTypes_.ManageStructVar (flag, arg, (structFunc_ && memberFuncs_.size () > 0) ? memberFuncs_.rbegin ()->second.struct_ : 0));
        else if (error)
        {
            *flag = ARG_ERROR;
            delete (std::string*) (*arg);
            *arg = 0;
        }
    }
    #undef $
}

void ScriptCompiler_t::ClassifyArg (Arg_t* arg, bool error, int line, bool var, bool struct_)
{
    _in_clsf_arg (&arg->flag1, &arg->arg1, error, line, var, struct_);
    _in_clsf_arg (&arg->flag2, &arg->arg2, error, line, var, struct_);
}

void ScriptCompiler_t::Dump ()
{
    printf ("file: %s\n", file_.c_str ());

    #define PRINT_MAP(name) \
    printf ("%s:\n", #name); \
    STL_LOOP (i, name) \
    { \
        printf ("  %s %lld\n", i->first.c_str (), i->second); \
    }
    PRINT_MAP (consts_)
    printf ("vars_:\n");
    STL_LOOP (i, vars_)
    {
        printf ("  %s %d %lld %d, %lld\n",
                i->first.first.c_str (),
                i->first.second,
                i->second.num,
                i->second.die,
                i->second.typeCode);
    }
    PRINT_MAP (userFuncs_)
    //PRINT_MAP (labels_)
    printf ("labels_:\n");
    STL_LOOP (i, labels_)
    {
        printf ("  %s %lld\n", i->first.c_str (), i->second);
    }

    printf ("strings_:\n");
    STL_LOOP (i, strings_)
    {
        printf ("  %s\n", i->c_str ());
    }

    printf ("funcs_:\n");
    STL_LOOP (i, funcs_)
    {
        printf ("  %d %lld\n", i->flag, i->cmd);
    }
    /*
    printf ("args_:\n");
    STL_LOOP (i, args_)
    {
        printf ("  %d %lld %d %lld\n", i->flag1, i->arg1, i->flag2, i->arg2);
    }
    */
    #undef PRINT_MAP
}

void ScriptCompiler_t::Save ()
{
    //Dump ();

    std::string saveFile (file_);
    size_t point = saveFile.rfind (".");
    if (point != saveFile.npos)
        saveFile.erase (saveFile.begin () + point, saveFile.end ());
    saveFile += ".pcs";

    FILE* save = fopen (saveFile.c_str (), "wb");
    if (!save)
    {
        ExceptionHandler* e = new (expn_) ExceptionHandler (E_NAT ( (std::string ("Unable to open file: ") + saveFile).c_str (), ERROR_SCRIPT_OPEN_FILE_SAVE));
        throw *e;
    }

    MapHeader maph = {KEY, VERSION};
    fwrite (&maph, sizeof (MapHeader), 1, save);

    #define WRITE_STR(name) \
        STL_LOOP (i, name) fwrite (& (*i), sizeof (char), 1, save); \
        fwrite (&CONTROL_CHARACTER, sizeof (CONTROL_CHARACTER), 1, save);

    #define WRITE_SIZE(name) \
        size_t size_##name = name.size (); \
        fwrite (&size_##name, sizeof (size_t), 1, save);


    #define WRITE_STL_LOOP(name, do) \
        WRITE_SIZE (name) \
        STL_LOOP (it, name) { do }

    #define WRITE_STL_LOOP_(name, var, do) \
        WRITE_SIZE (name) \
        STL_LOOP (var, name) { do }

    WRITE_STL_LOOP (dllImportMap_, {WRITE_STR (it->first);
                                   auto& vec = it->second;
                                   WRITE_STL_LOOP_ (vec, it_,
                                                   {WRITE_STR (it_->first);
                                                    fwrite (& (it_->second), sizeof (long long), 1, save);})})

    WRITE_STL_LOOP (typeSizes_, {fwrite (& (it->first), sizeof (long long), 1, save);
                                fwrite (& (it->second), sizeof (size_t), 1, save);})
    WRITE_STL_LOOP (vars_, {fwrite (& (it->second.num), sizeof (long long), 1, save);
                           fwrite (& (it->second.typeCode), sizeof (long long), 1, save);
                           fwrite (& (typeSizes_[it->second.typeCode]), sizeof (size_t), 1, save);})


    WRITE_STL_LOOP (userFuncs_, WRITE_STR (it->first))
    WRITE_STL_LOOP (strings_, WRITE_STR ( (*it)))
    assert (funcs_.size () == args_.size ());
    auto it1 = args_.begin ();
    WRITE_STL_LOOP (funcs_, {fwrite (& (*it), sizeof (Cmd_t), 1, save);
                            fwrite (& (*it1), sizeof (Arg_t), 1, save);
                            it1++;})

    fclose (save);

    #undef WRITE_STR
    #undef WRITE_SIZE
    #undef WRITE_STL_LOOP
    #undef WRITE_STL_LOOP_

}

void ScriptCompiler_t::ParseLinkFile (std::string file)
{
    FILE* f = fopen (file.c_str (), "r");
    if (!f)
    {
        static std::string str_error (std::string ("Unable to open file: ") + file);
        NAT_EXCEPTION (expn_, str_error.c_str (), ERROR_CONFLICTING_NAMES);
    }

    while (!feof (f))
    {
        ScriptLine_t line (f);
        Cmd_t cmd (line.cmd);
        Arg_t arg (line.param1, line.param2);

        #define LINK_CASE(name) case CMD_##name: Link##name (cmd, arg); break;

        switch (cmd.flag)
        {
            LINK_CASE (Null)
            LINK_CASE (Link)
            LINK_CASE (Const)
            LINK_CASE (Extern)
            default:
            ERROR_EXCEPTION ("Link: Unexpected cmd", ERROR_LINK_UNEXPECTED_CMD)
        }
    }

    fclose (f);
}

bool ScriptCompiler_t::CheckName (std::string name, int line)
{
    bool ok = true;
    auto varRes = vars_.find (StrTo32Pair_t (name, func_level_));
    auto fRes = createdFuncs_.end ();
    auto labRes = labels_.end ();
    if (ok && consts_.find (name) != consts_.end ()) ok = false;
    if (ok && varRes != vars_.end () && varRes->second.die > line) ok = false;
    if (ok && userFuncs_.find (name) != userFuncs_.end ()) ok = false;
    if (ok && ( (fRes = createdFuncs_.find (name)) != createdFuncs_.end ()) && fRes->second != -1) ok = false;
    if (ok && ( (labRes = labels_.find (name)) != labels_.end ()) && labRes->second != -1) ok = false;
    return ok;
}


bool ScriptCompiler_t::AddName (std::string name, char flag, long long cmd, int line)
{
    bool ok = true;

    if (ok)
    {
        switch (flag)
        {
            case CMD_Const:
                consts_[name] = cmd;
                break;
            case CMD_Label:
                if (labels_.find (name) == labels_.end ())
                {
                    labels_[name] = line;
                }
                else labels_[name] = line;
                break;
            case CMD_Extern:
                userFuncs_[name] = userFuncs_.size () - 1;
                break;
            case CMD_Var:
                vars_[StrTo32Pair_t (name, func_level_)] = {vars_.size () - 1, -1, TYPE_QWORD};
                break;
            case CMD_CFunc:
                createdFuncs_[name] = line;
                break;
        }
    }
    return ok;
}

bool ScriptCompiler_t::ManageStruct (Cmd_t& cmd, Arg_t& arg, int n_line)
{
    //if (cmd.cmd != CMD_Label && cmd.cmd != CMD_PLabel) ClassifyArg (&arg, false, n_line, true);

    //ClassifyArg (&arg, false, n_line, false, false);


    #define STRUCT_CASE(name) case CMD_##name: /*printf ("case %s\n", #name);*/ return Struct##name (cmd, arg, n_line);
    switch (cmd.flag)
    {
        STRUCT_CASE (Var)
        STRUCT_CASE (CFunc)
        STRUCT_CASE (Func)
        STRUCT_CASE (PFunc)
        STRUCT_CASE (Label)
        STRUCT_CASE (PLabel)
        STRUCT_CASE (Extern)
        STRUCT_CASE (Name)
        STRUCT_CASE (End_Struct)
        STRUCT_CASE (Struct)
        STRUCT_CASE (Null)
        default:
            ERROR_EXCEPTION ("Struct: Unexpected cmd", ERROR_LINK_UNEXPECTED_CMD)
    }
    return false;
}

void ScriptCompiler_t::ResolvePrototypes ()
{
    STL_LOOP (i, createdFuncs_)
    {
        if (i->second == -1)
        {
            static std::string str_err ("Undefined reference to function \"" + i->first + "\"");
            ERROR_EXCEPTION (str_err.c_str (), ERROR_UNDEFINED_REFERENCE_FUNC)
        }
    }
    STL_LOOP (i, labels_)
    {
        if (i->second == -1)
        {
            static std::string str_err ("Undefined reference to label \"" + i->first + "\"");
            ERROR_EXCEPTION (str_err.c_str (), ERROR_UNDEFINED_REFERENCE_LABEL)
        }
    }
    STL_LOOP (i, memberFuncs_)
    {
        if (i->second.func_ == -1)
        {
            static std::string str_err ("Undefined reference to member function \"" + i->first + "\"");
            ERROR_EXCEPTION (str_err.c_str (), ERROR_UNDEFINED_REFERENCE_FUNC)
        }
    }

    STL_LOOP (i, args_)
    {
        switch (i->flag1)
        {
            case ARG_FUNC_IT:
            {
                long long line = createdFuncs_[*(std::string*)i->arg1];
                delete (std::string*)i->arg1;
                i->arg1 = line;
                i->flag1 = ARG_FUNC;
                break;
            }
            case ARG_FUNC_MEMBER_IT:
            {
                auto found = memberFuncs_.find (*(std::string*) (i->arg1 >> 32));
                i->arg1 = found->second.func_;
                delete (std::string*) (i->arg1 >> 32);
                i->flag1 = ARG_FUNC_MEMBER;
                break;
            }
            case ARG_LABEL_IT:
            {
                long long line = labels_[*(std::string*)i->arg1];
                delete (std::string*)i->arg1;
                i->arg1 = line;
                i->flag1 = ARG_LABEL;
                break;
            }
        }
        switch (i->flag2)
        {
            case ARG_FUNC_IT:
            {
                long long line = createdFuncs_[*(std::string*)i->arg2];
                delete (std::string*)i->arg2;
                i->arg2 = line;
                i->flag2 = ARG_FUNC;
                break;
            }
            case ARG_FUNC_MEMBER_IT:
            {
                auto found = memberFuncs_.find (*(std::string*) (i->arg2 >> 32));
                i->arg2 = found->second.func_;
                delete (std::string*) (i->arg2 >> 32);
                i->flag2 = ARG_FUNC_MEMBER;
                break;
            }
            case ARG_LABEL_IT:
            {
                long long line = labels_[*(std::string*)i->arg2];
                delete (std::string*)i->arg2;
                i->arg2 = line;
                i->flag2 = ARG_LABEL;
                break;
            }
        }
    }
}
#undef ERROR_EXCEPTION
