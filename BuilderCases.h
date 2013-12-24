

#define _BUILD_CASE_FUNC(name) \
void ScriptCompiler_t::name (Cmd_t& cmd, Arg_t& arg, int n_line) \
{  \
n_line *= 1; cmd.cmd *= 1;

#define _RETURN \
}

#define CHECK_ARGS(expr, func) \
if (expr) ERROR_EXCEPTION ("Invalid args for \"" func "\"", ERROR_PARSER_INVALID_ARGS)

#define CHECK_TOKEN(tok) \
if (!CheckName (tok, n_line)) STRING_ERROR (tok)

#define STD_TOK (* reinterpret_cast <std::string*> (arg.arg1))

#define CHECK_STD_TOKEN \
CHECK_TOKEN (STD_TOK)

#define STRING_ERROR(name) \
{ \
    char line_ [6] = ""; \
    _itoa (n_line, line_, 10); \
    static std::string str_error (std::string ("String ") + name + " has already been registered " + " (line " + line_ + ")"); \
    NAT_EXCEPTION (expn_, str_error.c_str (), ERROR_CONFLICTING_NAMES); \
}

#define ADD_TOKEN(name) \
AddName (name, cmd.flag, cmd.cmd, n_line);

#define ADD_TOKEN_(name, val) \
AddName (name, cmd.flag, val, n_line);

#define ADD_STD_TOKEN ADD_TOKEN (STD_TOK)
#define ADD_STD_TOKEN_(val) ADD_TOKEN_ (STD_TOK, val)

#define ERROR_EXCEPTION(text, code)\
{ \
    char line_ [6] = ""; \
    _itoa (n_line, line_, 10); \
    static std::string str_error ("Error occured while parsing file \"" + file_ + \
                                  "\" on line " + line_ + ": " + text); \
    NAT_EXCEPTION (expn_, str_error.c_str (), code)\
}

_BUILD_CASE_FUNC (Null)
    cmd.Clear ();
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Func)
    bool struct_ = structFunc_;
    if (cmd.cmd == CMD_Ret)
    {
        if (structFunc_) currFunc_.clear ();
        func_level_--;
        ActivateDieRequests (n_line);
        structFunc_ = false;
    }
    ClassifyArg (&arg, true, n_line, true, struct_);
_RETURN

_BUILD_CASE_FUNC (Struct)
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "struct")
    cmd.Clear ();
    CHECK_STD_TOKEN
    currStruct_ = regTypes_.AddNewType (STD_TOK);
    arg.Clear ();
    struct_func_level_ = func_level_;
_RETURN

_BUILD_CASE_FUNC (Link)
    ClassifyArg (&arg, true, n_line);
    CHECK_ARGS (arg.flag1 != ARG_STR ||
                arg.flag2 != ARG_NULL, "link")
    ParseLinkFile (strings_[arg.arg1]);
    cmd.Clear ();
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (CFunc)
    if (func_level_ > 0)
        ERROR_EXCEPTION ("Cannot create nested function", ERROR_NESTED_FUNC)
    func_level_++;
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "func")
    CHECK_STD_TOKEN
    ADD_STD_TOKEN
    currFunc_ = STD_TOK;
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Label)
    CHECK_ARGS (arg.flag1 != ARG_NULL ||
                arg.flag2 != ARG_NULL, "label")
    arg.Clear ();
    CHECK_TOKEN (* reinterpret_cast <std::string*> (cmd.cmd))
    ADD_TOKEN (* reinterpret_cast <std::string*> (cmd.cmd))
    cmd.Clear ();
_RETURN

_BUILD_CASE_FUNC (Const)
    ClassifyArg (&arg, false, n_line);
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "const")
    ADD_STD_TOKEN_ (arg.arg2)
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Extern)
    ClassifyArg (&arg, false, n_line);
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "extern")
    CHECK_STD_TOKEN
    ADD_STD_TOKEN
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Import)
    ClassifyArg (&arg, false, n_line);
    CHECK_ARGS (arg.flag1 != ARG_STR ||
                arg.flag2 != ARG_STR, "import")

    int num = 0;
    STL_LOOP (i, dllImportMap_) num += static_cast <int> (i->second.size ());

    std::string funcName (strings_[arg.arg2].begin (),
                          strings_[arg.arg2].begin () +
                          strings_[arg.arg2].find (' '));
    std::string useName  (strings_[arg.arg2].begin () +
                          strings_[arg.arg2].find (' ') + 1,
                          strings_[arg.arg2].end ());

    if (funcName.empty () || useName.empty ())
        ERROR_EXCEPTION ("Error in import function definition",
                         ERROR_IMPORT_DLL_INVALID_STR)
    {
        auto foundIt = dllImportMap_.end ();
        if ((foundIt = dllImportMap_.find (strings_[arg.arg1])) ==
             dllImportMap_.end ())
        {
            DllVector_t vec;
            vec.push_back (DllPair_t (funcName, num));
            dllImportMap_[strings_[arg.arg1]] = vec;
        }
        else
            foundIt->second.push_back (DllPair_t (funcName, num));
    }

    {
        if (dllFuncsMap_.find (useName) !=
            dllFuncsMap_.end ()) STRING_ERROR (useName)
        dllFuncsMap_[useName] = num;
    }
    arg.Clear ();

_RETURN

_BUILD_CASE_FUNC (Var)
    //ErrorPrintfBox ("A Var");
    ClassifyArg (&arg, false, n_line, false);
    //ErrorPrintfBox ("B Var");
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
               (arg.flag2 != ARG_NULL &&
                arg.flag2 != ARG_NUM) ||
               (arg.flag2 == ARG_NUM &&
                typeSizes_.find (arg.flag2) == typeSizes_.end ()),
                "var")
    //ErrorPrintfBox ("C Var");
    auto tempRes = vars_.find (StrTo32Pair_t (STD_TOK, func_level_));
    if (tempRes != vars_.end () &&
       ((func_level_ != 0 && tempRes->second.die == -1) ||
         tempRes->second.die > n_line))
        ERROR_EXCEPTION ("Var already exists", ERROR_VAR_ALREADY_EXISTS)

        //! Check if this var has already been created

    //ErrorPrintfBox ("D Var");
    if (tempRes != vars_.end () && tempRes->second.die < n_line && tempRes->second.die != -1)
    { //! if yes rebuild the var with new type
        cmd.Set (CMD_Func, CMD_RebuildVar);
        int8_t flag = arg.flag2;
        int64_t type = arg.arg2;
        tempRes->second.die = -1;
        if (func_level_ > 0)
        {
            CreateVarDieRequest (tempRes);
        }
        arg.Clear ();
        arg.Set (ARG_VAR, tempRes->second.num, flag, type);
    }
    else //! if not create a new one
    {
        CHECK_STD_TOKEN
        ADD_STD_TOKEN
        auto foundIter = vars_.find (StrTo32Pair_t (STD_TOK, func_level_));
        if (arg.flag2 == ARG_NUM) foundIter->second.typeCode = arg.arg2;

        if (func_level_ > 0)
            CreateVarDieRequest (foundIter);
        cmd.Clear ();
        arg.Clear ();
    }
    //ErrorPrintfBox ("E Var");
_RETURN

_BUILD_CASE_FUNC (Name)
    ClassifyArg (&arg, true, n_line);
    auto result = userFuncs_.end ();
    if ((result = userFuncs_.find (* reinterpret_cast<std::string*> (cmd.cmd))) != userFuncs_.end ())
    {
        cmd.flag = CMD_UFunc;
        cmd.cmd = result->second;
    }
    else
        ERROR_EXCEPTION ("Unable to recognize the cmd", ERROR_NOT_RECOGNIZED_CMD)
_RETURN

_BUILD_CASE_FUNC (PFunc)
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "pfunc")
    CHECK_STD_TOKEN
    createdFuncs_[STD_TOK] = -1;
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (PLabel)
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "plabel")
    CHECK_STD_TOKEN
    labels_[STD_TOK] = -1;
    arg.Clear ();
    cmd.Clear ();
_RETURN

#undef _BUILD_CASE_FUNC
#define _BUILD_CASE_FUNC(name) \
bool ScriptCompiler_t::Struct##name (Cmd_t& cmd, Arg_t& arg, int n_line) \
{ \
cmd.cmd *= 1; arg.arg1 *= 1; n_line *= 1;

#undef CHECK_ARGS
#define CHECK_ARGS(expr, func) \
if (expr) ERROR_EXCEPTION ("Struct: invalid args for \"" func "\"", ERROR_PARSER_INVALID_ARGS)

_BUILD_CASE_FUNC (Var)
    ClassifyArg (&arg, false, n_line, false, true);
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
               (arg.flag2 != ARG_NULL &&
                arg.flag2 != ARG_NUM) ||
               (arg.flag2 == ARG_NUM &&
                typeSizes_.find (arg.flag2) == typeSizes_.end ()),
                "var")
    CHECK_STD_TOKEN
    bool res = 0;
    if (arg.flag2 == ARG_NULL)
        res = regTypes_.AddVar (STD_TOK, currStruct_, TYPE_QWORD);
    else
        res = regTypes_.AddVar (STD_TOK, currStruct_, arg.arg2);

    if (!res)
        ERROR_EXCEPTION ("Struct: error occured while adding variable to struct", ERROR_ADDING_VAR_STRUCT)
    cmd.Clear ();
    arg.Clear ();
    return true;
_RETURN

_BUILD_CASE_FUNC (CFunc)
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL,
                "func")
    if (structFunc_)
        ERROR_EXCEPTION ("Struct: Cannot create nested function", ERROR_NESTED_FUNC)
    regTypes_.AddFunc (STD_TOK, n_line, currStruct_);
    structFunc_ = true;
    arg.Clear ();
    return true;
_RETURN

_BUILD_CASE_FUNC (Func)
    return false;
_RETURN

_BUILD_CASE_FUNC (PFunc)
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "pfunc")
    CHECK_STD_TOKEN
    assert (regTypes_.AddFunc (STD_TOK, -1, currStruct_));
    arg.Clear ();
    return true;
_RETURN

_BUILD_CASE_FUNC (Label)
    CHECK_ARGS (arg.flag1 != ARG_NULL ||
                arg.flag2 != ARG_NULL, "label")
    arg.Clear ();
    CHECK_TOKEN (* reinterpret_cast<std::string*> (cmd.cmd))
    ADD_TOKEN (* reinterpret_cast<std::string*> (cmd.cmd))
    cmd.Clear ();
    return true;
_RETURN

_BUILD_CASE_FUNC (PLabel)
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL, "plabel")
    CHECK_STD_TOKEN
    labels_[STD_TOK] = -1;
    arg.Clear ();
    cmd.Clear ();
    return true;
_RETURN

_BUILD_CASE_FUNC (Extern)
    return false;
_RETURN

_BUILD_CASE_FUNC (Name)
    return false;
_RETURN

_BUILD_CASE_FUNC (Struct)
    ERROR_EXCEPTION ("Struct: Cannot create nested struct", ERROR_NESTED_STRUCT)
    return false;
_RETURN

_BUILD_CASE_FUNC (End_Struct)
    CHECK_ARGS (arg.flag1 != ARG_NULL ||
                arg.flag2 != ARG_NULL,
                "end_struct")
    cmd.Clear ();
    currStruct_ = 0;
    arg.Clear ();
    return false;
_RETURN

_BUILD_CASE_FUNC (Null)
    cmd.Clear ();
    arg.Clear ();
    return true;
_RETURN

///-----------------------------------------------------------------
#undef _BUILD_CASE_FUNC
#define _BUILD_CASE_FUNC(name) \
void ScriptCompiler_t::Link##name (Cmd_t& cmd, Arg_t& arg) \
{

#undef CHECK_ARGS
#define CHECK_ARGS(expr, func) \
if (expr) ERROR_EXCEPTION ("Link: invalid args for \"" func "\"", ERROR_PARSER_INVALID_ARGS)

#undef STRING_ERROR
#define STRING_ERROR(name) \
{ \
    static std::string str_error (std::string ("String ") + name + " has already been registered "); \
    NAT_EXCEPTION (expn_, str_error.c_str (), ERROR_CONFLICTING_NAMES); \
}

#undef CHECK_TOKEN
#define CHECK_TOKEN(tok) \
if (!CheckName (tok, 0)) STRING_ERROR (tok)

#undef ADD_TOKEN
#define ADD_TOKEN(name) \
AddName (name, cmd.flag, cmd.cmd, 0);

#undef ADD_TOKEN_
#define ADD_TOKEN_(name, val) \
AddName (name, cmd.flag, val, 0);

#undef ERROR_EXCEPTION
#define ERROR_EXCEPTION(text, code)\
{ \
    static std::string str_error ("Error occured while parsing file \"" + file_ + \
                                  "\": " + text); \
    NAT_EXCEPTION (expn_, str_error.c_str (), code)\
}

_BUILD_CASE_FUNC (Null)
    cmd.Clear ();
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Link)
    ClassifyArg (&arg, true, -1);
    CHECK_ARGS (arg.flag1 != ARG_STR ||
                arg.flag2 != ARG_NULL,
                "link")
    ParseLinkFile (* reinterpret_cast <std::string*> (arg.arg1));
    cmd.Clear ();
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Const)
    ClassifyArg (&arg, false, -1);
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL,
                "const")
    CHECK_STD_TOKEN
    ADD_STD_TOKEN_ (arg.arg2)
    arg.Clear ();
_RETURN

_BUILD_CASE_FUNC (Extern)
    ClassifyArg (&arg, false, -1);
    CHECK_ARGS (arg.flag1 != ARG_NAME ||
                arg.flag2 != ARG_NULL,
                "extern")
    CHECK_STD_TOKEN
    ADD_STD_TOKEN
    arg.Clear ();
_RETURN


