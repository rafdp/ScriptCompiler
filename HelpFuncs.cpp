
#define RET_NO_ERRORS ErrorReturn_t (RET_SUCCESS)

#define CATCH_CONS(data, message, code) \
catch (ExceptionHandler& e) \
    CONS_EXCEPTION (expn_, message, code, e)

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))

class StackData_t;

bool isNum (std::string* str);
bool IsString (std::string* str);
bool IsArgValue (const char& flag);
char _GetString (FILE* f, std::string* str, char del);

std::string GetAsmNumString (int val, const char* operand = "dword");
void PushStackValueJit (const StackData_t& value, JitCompiler_t* comp, exception_data* expn);



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
    return (*(str->begin ()) == '"' && *(str->rbegin ()) == '"');
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

    VarData_t (void* var_, long long code_, size_t size_) :
        var  (var_),
        code (code_),
        size (size_)
    {}

    VarData_t (const VarData_t& that) :
        var  (that.var),
        code (that.code),
        size (that.size)
    {}

    VarData_t& operator = (const VarData_t& that)
    {
        var  = that.var;
        code = that.code;
        size = that.size;
        return *this;
    }

    VarData_t () :
        var  (nullptr),
        code (0),
        size (0)
    {}

    void Free ()
    {
        if (size > 0) var = new char [size];
    }

    void Delete ()
    {
        if (var) delete [] reinterpret_cast<char*> (var);
        var = nullptr;
    }

    ~VarData_t ()
    {
        Delete ();
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

    ErrorReturn_t () :
        retVal (),
        errorText ()
    {}

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
    while (c == ' ') c = static_cast<char> (fgetc (f));
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
            switch (c = static_cast<char> (fgetc (f)))
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
        c = static_cast<char> (fgetc (f));
        iteration++;
    }
    return c;
}

struct ExpectedArg_t
{
    std::vector<char> expFlag1;
    std::vector<char> expFlag2;

    ExpectedArg_t (int n_flag1, int n_flag2, ...) :
        expFlag1 (),
        expFlag2 ()
    {
        va_list arg; va_start (arg, n_flag2);
        for (int i = 0; i < n_flag1; i++) expFlag1.push_back (char(va_arg (arg, int)));
        for (int i = 0; i < n_flag2; i++) expFlag2.push_back (char(va_arg (arg, int)));
        va_end (arg);

        if (n_flag1 == 0) expFlag1.push_back (ARG_NULL);
        if (n_flag2 == 0) expFlag2.push_back (ARG_NULL);
    }

    ~ExpectedArg_t ()
    { }
};

template<class T> typename std::vector<T>::const_iterator find (const std::vector<T>& vec, const T& data)
{
    STL_LOOP (i, vec) if (*i == data) return i;
    return vec.end ();
}

struct StackData_t
{
    long long data;
    size_t size;

    template <typename T>
    StackData_t (T data_ = 0, size_t size_ = 0) :
        data (static_cast <int64_t> (data_)),
        size (size_)
    {}

    bool operator! ()
    {
        return (data == 0);
    }

    template <typename T>
    operator T ()
    {
        return reinterpret_cast <T> (data);
    }

    friend std::ostream& operator<< (std::ostream& output, const StackData_t& s)
    {
        output << s.data;
        return output;
    }
};

std::string GetAsmNumString (int val, const char* operand /* = "dword" */)
{
    char val_[MAX_PATH] = "";
    itoa (val, val_, 16);

    std::string val_str;
    val_str += std::string (operand) + " (0" + val_ + "h)";
    return val_str;
}

void PushStackValueJit (const StackData_t& value, JitCompiler_t* comp, exception_data* expn)
{
    //ErrorPrintfBox ("%d %s", value.size, __PRETTY_FUNCTION__);
    switch (value.size)
    {
        case sizeof (int8_t):
        comp->push (static_cast<int8_t> (value.data));
        break;
        case sizeof (int16_t):
        comp->push (static_cast<int16_t> (value.data));
        break;
        case sizeof (int32_t):
        comp->push (static_cast<int32_t> (value.data));
        break;
        case sizeof (int64_t):
        comp->push (static_cast<int64_t> (value.data));
        break;
        default:
        NAT_EXCEPTION (expn, "Failed to emit \"push\", invalid operand size", ERROR_INVALID_OP_SIZE_SWITCH)
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

struct CallInfo_t
{
    int retLine;
    int var;
    CallInfo_t (int retLine_ = 0, int var_ = 0) :
        retLine (retLine_),
        var (var_)
    {}

    friend std::ostream& operator<< (std::ostream& output, const CallInfo_t& s)
    {
        output << s.retLine;
        return output;
    }
};

struct JmpPatchData_t
{
    int64_t first, second;
    bool offset;
};

