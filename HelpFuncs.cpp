
#define STL_LOOP(iterator, object) for (auto iterator = (object).begin(); iterator != (object).end(); iterator++)
#define STL_RLOOP(iterator, object) for (auto iterator = (object).begin(); iterator != (object).end(); iterator++)

#define RET_NO_ERRORS ErrorReturn_t (RET_SUCCESS)

#define NAT_EXCEPTION(data, message, code) \
{ \
    ExceptionHandler* e_nat = new (data) ExceptionHandler (E_NAT((message), (code))); \
    throw *e_nat; \
}

#define CONS_EXCEPTION(data, message, code, old) \
{ \
    ExceptionHandler* ec = new (data) ExceptionHandler (E_CONS((message), (code), (old).pt_)); \
    throw *ec; \
}
#define CATCH_CONS(data, message, code) \
catch (ExceptionHandler& e) \
    CONS_EXCEPTION (expn_, "Parser crashed", ERROR_PARSER_CRASHED, e)


bool isNum (std::string* str)
{
    bool first = true;
    STL_LOOP (i, *str)
    {
        if (first && (*i != '-' && *i != '+') && !isdigit (*i))
            return false;
        else
        {
            first = false;
            continue;
        }

        if (!first && !isdigit (*i))
            return false;
    }
    return true;
}

bool IsString (std::string* str)
{
    //printf ("ISSTRING\n");
    return (*(str->begin()) == '"' && *(str->rbegin()) == '"');
}

struct VarDescriptor_t
{
    long long num;
    short die;
    long long typeCode;
    bool pointer;
};

struct VarData_t
{
    void* var;
    long long code;
    size_t size;
    //char ptrFlag;

    VarData_t (void* var_, long long code_, size_t size_) :
        var  (var_),
        code (code_),
        size (size_)//,
        //ptrFlag ()
    {}

    void Free()
    {
        if (size > 0) var = new char [size];
    }

    void Delete()
    {
        if (var) delete [] (char*)var;
        var = nullptr;
    }

    ~VarData_t ()
    {
        Delete();
    }
};


bool IsArgValue (const char& flag)
{
    if (flag != ARG_NUM &&
        flag != ARG_VAR &&
        flag != ARG_REG &&
        flag != ARG_STR)
        return false;

    return true;
}

struct ErrorReturn_t
{
    int retVal;
    std::string errorText;
    ErrorReturn_t (int val) :
        retVal (val),
        errorText ()
    {}
    ErrorReturn_t (int val, const char* text) :
        retVal (val),
        errorText (text)
    {}
    ErrorReturn_t (int val, std::string text) :
        retVal (val),
        errorText (text)
    {}
};

char _GetString (FILE* f, std::string* str, char del)
{
    char c = ' ';
    bool str_ = false;
    bool str_found = false;
    while (c == ' ') c = fgetc (f);
    int iteration = 0;
    while (c != '\n'          &&
           c != EOF           &&
           (str_ || c != ' ') &&
           c != del && (!str_found || str))
    {
        if (c == '"' && iteration == 0) {str_ = true; str_found = true;}
        else if (c == '"') str_ = false;
        if (str_ && c == '\\')
        {
            switch (c = fgetc(f))
            {
                case 'n':
                    *str += '\n';
                    break;
                case 'r':
                    *str += '\r';
                    break;
                case '\\':
                    *str += '\\';
                    break;
                case '"':
                    *str += '"';
                    str_ = true;
                    break;
                default:
                    *str += '\\';
                    *str += c;
                    break;
            }
        }
        else
        {
            *str += c;
        }
        c = fgetc (f);
        iteration++;
    }
    return c;
}

int ErrorPrintfBox (HWND wnd, DWORD flags, const char* format, ...)
{
    if (!format) return 0;

    char str[4096] = "";

    va_list arg; va_start (arg, format);
    int n = _vsnprintf (str, sizeof (str) - 1, format, arg);
    va_end (arg);

    MessageBoxA (wnd, str, "", flags);
    return n;
}

struct ExpectedArg_t
{
    std::vector<char> expFlag1;
    std::vector<char> expFlag2;

    ExpectedArg_t (int n_flag1, int n_flag2, ...)
    {
        va_list arg; va_start (arg, n_flag2);
        for (int i = 0; i < n_flag1; i++) expFlag1.push_back(va_arg (arg, int));
        for (int i = 0; i < n_flag2; i++) expFlag2.push_back(va_arg (arg, int));
        va_end (arg);

        if (n_flag1 == 0) expFlag1.push_back (ARG_NULL);
        if (n_flag2 == 0) expFlag2.push_back (ARG_NULL);
    }
};

template<class T> typename std::vector<T>::const_iterator find(const std::vector<T>& vec, const T& data)
{
    STL_LOOP (i, vec) if (*i == data) return i;
    return vec.end();
}

struct StackData_t
{
    long long data;
    size_t size;

    template <typename T>
    StackData_t (T data_ = 0, size_t size_ = 0) :
        data ((long long) data_),
        size (size_)
    {}

    bool operator! ()
    {return data == 0;}

    template <typename T>
    operator T ()
    {return (T)data;}

    friend std::ostream& operator<< (std::ostream& output, const StackData_t& s)
    {
        output << s.data;
        return output;
    }
};

int RunAsm (std::string code)
{
    {
        FILE* compileAsm = fopen("fasm_compile.asm", "w");
        assert (compileAsm);

        code.push_back(0);
        fprintf (compileAsm, "format pe\n");

        fwrite (code.c_str(), sizeof (char), code.size(), compileAsm);

        fclose (compileAsm);

        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        SetStdHandle (STD_OUTPUT_HANDLE, 0);
        system ("fasm.exe fasm_compile.asm");
        SetStdHandle (STD_OUTPUT_HANDLE, h);
    }
    {
        FILE* compiledAsm = fopen("fasm_compile.exe", "rb");
        assert (compiledAsm);

        while (!feof (compiledAsm)) fgetc (compiledAsm);

        int size = ftell (compiledAsm) - 0x200;

        rewind (compiledAsm);

        for (int i = 0; i < 0x200; i++) fgetc (compiledAsm);


        unsigned char* func = new unsigned char [size + 1];

        fread (func, sizeof (char), size, compiledAsm);

        //printf ("%X\n", *func);

        //int ret = ((int (*) ())func) ();
        ((void (*) ())func) ();
        delete[] func;
        fclose (compiledAsm);

        //remove ("fasm_compile.asm");
        //remove ("fasm_compile.exe");

        //return ret;
    }
}

std::string GetAsmNumString (int val, const char* operand = "dword")
{
    char val_[MAX_PATH] = "";
    itoa (val, val_, 16);
    std::string val_str;
    val_str += std::string(operand) + "(0" + val_ + "h)";
    return val_str;
}

void PushStackValueString (const StackData_t& value, std::string* str)
{
    std::string type;
    if (value.size <= sizeof (int))
    {
        switch (value.size)
        {
            case sizeof (char):
            type = "byte";
            break;
            case sizeof (short):
            type = "word";
            break;
            case sizeof (int):
            type = "dword";
            break;
            //!default:
            //!NAT_EXCEPTION ()
        }
        (*str) += "push " + GetAsmNumString (value.data, type.c_str()) + "\n";
    }
    else
    if (value.size == sizeof (long long))
    {
        (*str) += "push " + GetAsmNumString (*((int*)(&(value.data)) + 1), "") + "\n";
        (*str) += "push " + GetAsmNumString (*((int*)(&(value.data))), "") + "\n";
        // (*str) += "push " + GetAsmNumString (int(&(value.data)), "qword") + "\n";
    }

}

void PushStackValueJit (const StackData_t& value, JitCompiler_t* comp)
{
    std::string type;
    if (value.size <= sizeof (int))
    {
        switch (value.size)
        {
            case sizeof (char):
            case sizeof (short):
            comp->PushWord (WORD (value.data));
            break;
            case sizeof (int):
            comp->PushDword (DWORD (value.data));
            break;
            //!default:
            //!NAT_EXCEPTION ()
        }

    }
    else
    if (value.size == sizeof (long long))
    {
        comp->PushDword (DWORD (*((int*)(&(value.data)) + 1)));
        comp->PushDword (DWORD (*((int*)(&(value.data)) + 0)));
    }

}

struct MemberVarDescriptor_t
{
    long long num;
    long long offset;
    long long typeCode;
};

struct FuncStructPair_t
{
    long long func_;
    long long struct_;
};

struct LabelPair_t
{
    long long line;
    long long num;
};

struct CallInfo_t
{
    int retLine;
    long long var;
    CallInfo_t (int retLine_ = 0, long long var_ = 0) :
        retLine (retLine_),
        var (var_)
    {}
};

