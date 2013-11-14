
class FileHandler_t
{
    public:
    FILE* file;

    template<typename T>
    T GetData ()
    {
        //T data = {};
        T data;
        fread (&data, sizeof (T), 1, file);
        return data;
    }

};

class ScriptLoader_t
{
public:
    std::map<long long, size_t> typeSizes_;
    std::vector<VarData_t>      vars_;
    std::vector<std::string>    userFuncs_;
    std::vector<char*>          strings_;
    std::vector<Cmd_t>          funcs_;
    std::vector<Arg_t>          args_;
    DllImportMap_t              dllFuncs_;
    ResolvedDllFuncsMap_t       dllResolved_;
    exception_data*             expn_;

    ScriptLoader_t (std::string filename, exception_data* expn);

    bool CheckHeader (FILE* f);

    void Dump ();
    virtual ~ScriptLoader_t ()
    {
        STL_LOOP (i, strings_) delete[] *i;
    }

    void ResolveDlls ();
};

ScriptLoader_t::ScriptLoader_t (std::string filename, exception_data* expn) :
    typeSizes_ (),
    vars_      (),
    userFuncs_ (),
    strings_   (),
    funcs_     (),
    args_      (),
    dllFuncs_  (),
    expn_      (expn)
{
    FILE* script = fopen (filename.c_str(), "rb");
    if (!script)
    {
        ExceptionHandler* e = new (expn) ExceptionHandler (E_NAT((std::string ("Unable to open file: ") + filename).c_str(), ERROR_SCRIPT_OPEN_FILE_CTOR));
        throw *e;
    }
    if (!CheckHeader (script))
    {
        ExceptionHandler* e = new (expn) ExceptionHandler (E_NAT((std::string ("Invalid script file: ") + filename).c_str(), ERROR_INVALID_HEADER));
        throw *e;
    }

    FileHandler_t fileHandler = {script};

    #define _load(type) fileHandler.GetData<type>()

    #define IMPORT_LOOP(var, action) \
    size_t n_##var = _load(size_t); \
    for (size_t i = 0; i < n_##var; i++) \
    { \
        action \
    }
    #define IMPORT_LOOP_(var, iter, action) \
    size_t n_##var = _load(size_t); \
    for (size_t iter = 0; iter < n_##var; iter++) \
    { \
        action \
    }
    #define IMPORT_LOOP__(var, preaction, action) \
    size_t n_##var = _load(size_t); \
    preaction \
    for (size_t iter = 0; iter < n_##var; iter++) \
    { \
        action \
    }

    #define READ_STR(name) \
    std::string name; \
    char c = 0; \
    while ((c = _load(char)) != CONTROL_CHARACTER) \
        name += c;

    IMPORT_LOOP(dllFuncs_,
                {READ_STR(dll)
                 dllFuncs_[dll] = DllVector_t();
                 DllVector_t& currVec = dllFuncs_[dll];
                 IMPORT_LOOP_(currVec,
                              it,
                              {READ_STR(funcName)
                              currVec.push_back (DllPair_t (funcName, _load(long long)));})})


    IMPORT_LOOP(typeSizes, {long long code = _load(long long);
                            typeSizes_[code] = _load (size_t);})

    IMPORT_LOOP__(vars, {vars_.reserve (n_vars); for (size_t i = 0; i < n_vars; i++) vars_.push_back(VarData_t());},
                  {long long num = _load(long long);
                   long long code = _load(long long);
                   vars_[num] = VarData_t(nullptr, code, _load (size_t));})


    IMPORT_LOOP(userFuncs, {READ_STR(currentStr)
                            userFuncs_.push_back (currentStr);})

    IMPORT_LOOP(strings, {READ_STR(currentStr)
                          strings_.push_back (new char[currentStr.size() > STRING_SIZE ? currentStr.size() + 1 : STRING_SIZE]);
                          strcpy (*strings_.rbegin(), currentStr.c_str());})

    IMPORT_LOOP(funcs, {funcs_.push_back (_load(Cmd_t));
                        args_.push_back (_load(Arg_t));})


    #undef _load

    #undef IMPORT_LOOP
    #undef IMPORT_LOOP_
    #undef IMPORT_LOOP__
    #undef READ_STR

    fclose (script);

    ResolveDlls ();
}

bool ScriptLoader_t::CheckHeader (FILE* f)
{
    static MapHeader headerGood = {KEY, VERSION};
    MapHeader header = {};
    fread (&header, sizeof (MapHeader), 1, f);
    if (header != headerGood) return false;
    return true;
}


void ScriptLoader_t::Dump ()
{
    //printf ("n_vars: %d\n", n_vars_);
    printf ("userFuncs: %d\n", userFuncs_.size());
    for (auto i = userFuncs_.begin(); i < userFuncs_.end(); i++)
        printf ("  \"%s\"\n", i->c_str());
    printf ("strings: %d\n", strings_.size());
    for (auto i = strings_.begin(); i < strings_.end(); i++)
        printf ("  \"%s\"\n", *i);

    printf ("funcs_:\n");
    for (auto i = funcs_.begin(); i < funcs_.end(); i++)
    {
        printf ("  %d %lld\n", i->flag, i->cmd);
    }

    printf ("args_:\n");
    for (auto i = args_.begin(); i < args_.end(); i++)
    {
        printf ("  %d %lld %d %lld\n", i->flag1, i->arg1, i->flag2, i->arg2);
    }
}

void ScriptLoader_t::ResolveDlls ()
{
    STL_LOOP (i, dllFuncs_)
    {
        HMODULE dll = LoadLibrary (i->first.c_str());
        if (!dll)
        {
            static std::string str ("Cannot find dll:\"");
            str += i->first;
            str += '"';
            NAT_EXCEPTION (expn_, str.c_str(), ERROR_NOT_FOUND_DLL)
        }
        STL_LOOP (iter, i->second)
        {
            void* func = (void*) GetProcAddress (dll, iter->first.c_str());
            if (!func)
            {
                static std::string str ("Cannot find function:\"");
                str += iter->first;
                str += "\" in dll:\"";
                str += i->first;
                str += '"';
                NAT_EXCEPTION (expn_, str.c_str(), ERROR_NOT_FOUND_DLL_FUNC)
            }
            dllResolved_[iter->second] = func;
        }
        FreeLibrary (dll);
    }
}

