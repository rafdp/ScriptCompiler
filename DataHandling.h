
class RunInstanceDataHandler_t;

struct UserFunc_t
{
    void (*func) (RunInstanceDataHandler_t*, void*);
    void* ptr;
    UserFunc_t (void (*f) (RunInstanceDataHandler_t*, void*) = nullptr, void* pt = nullptr) :
        func (f),
        ptr  (pt)
    {}

    void CallFunc (RunInstanceDataHandler_t* instance)
    {
        func (instance, ptr);
    }
};

class RunInstanceDataHandler_t : public ScriptLoader_t
{
    DISABLE_CLASS_COPY (RunInstanceDataHandler_t)
public:
    stack<StackData_t>      dataStack_;
    stack<CallInfo_t>       callStack_;
    char                    registers_[REGISTERS_SIZE];
    size_t                  run_line_;
    char                    cmpr_flag_;
    std::map<long long,
             UserFunc_t>    callImportFuncs_;
    int                     stackDumpPoint_;
    int64_t*                func_offsets_;

    RunInstanceDataHandler_t (std::string filename,
                              exception_data* expn,
                              std::map<std::string, UserFunc_t> importUserFuncs);

    long long GetVal (char arg_flag, long long arg_arg);
    bool SetVal (char arg_flag, long long arg_arg, long long setting);

    RegInfo_t GetReg (long long param)
    {
        return RegInfo_t (registers_, param, expn_);
    }

    void CallImportFunc (long long func)
    {
        return callImportFuncs_[func].CallFunc (this);
    }

    long long GetVar (long long num)
    {
        switch (vars_[num].code)
        {
        case TYPE_BYTE:
            return * reinterpret_cast<int8_t*>  (vars_[num].var);
        case TYPE_WORD:
            return * reinterpret_cast<int16_t*> (vars_[num].var);
        case TYPE_DWORD:
        case TYPE_PTR:
            return * reinterpret_cast<int32_t*> (vars_[num].var);
        case TYPE_QWORD:
            return * reinterpret_cast<int64_t*> (vars_[num].var);
        default:
            return 0;
        }
    }

    long long GetMemberVar (long long arg)
    {
        short typeCode = static_cast <int16_t> (MemberVarType (arg));
        switch (typeCode)
        {
            case TYPE_BYTE:
                return * reinterpret_cast<int8_t*>  (MemberVarPt (arg));
            case TYPE_WORD:
                return * reinterpret_cast<int16_t*> (MemberVarPt (arg));
            case TYPE_DWORD:
            case TYPE_PTR:
                return * reinterpret_cast<int32_t*> (MemberVarPt (arg));
            case TYPE_QWORD:
                return * reinterpret_cast<int64_t*> (MemberVarPt (arg));
            default:
                return 0;
        }
    }

    void SetVar (long long num, long long val)
    {
        switch (vars_[num].code)
        {
            case TYPE_BYTE:
                * reinterpret_cast<int8_t*> (vars_[num].var) =  static_cast <int8_t> (val);
                break;
            case TYPE_WORD:
                * reinterpret_cast<int16_t*> (vars_[num].var) = static_cast <int16_t> (val);
                break;
            case TYPE_DWORD:
            case TYPE_PTR:
                * reinterpret_cast<int32_t*> (vars_[num].var) = static_cast <int32_t> (val);
                break;
            case TYPE_QWORD:
                * reinterpret_cast<int64_t*> (vars_[num].var) = static_cast <int64_t> (val);
                break;
            default:
                return;
        }
    }

    void SetMemberVar (long long arg, long long val)
    {
        short typeCode = static_cast <int16_t> (MemberVarType (arg));

        switch (typeCode)
        {
            case TYPE_BYTE:
                * reinterpret_cast<int8_t*> (MemberVarPt (arg)) =  static_cast <int8_t> (val);
                break;
            case TYPE_WORD:
                * reinterpret_cast<int16_t*> (MemberVarPt (arg)) = static_cast <int16_t> (val);
                break;
            case TYPE_DWORD:
            case TYPE_PTR:
                * reinterpret_cast<int32_t*> (MemberVarPt (arg)) = static_cast <int32_t> (val);
                break;
            case TYPE_QWORD:
                * reinterpret_cast<int64_t*> (MemberVarPt (arg)) = static_cast <int64_t> (val);
                break;
            default:
                return;
        }
    }

    long long MemberVarType (long long arg)
    {
        return short (arg >> 48);
    }

    void* MemberVarPt (long long arg)
    {
        int num = short (arg >> 32);
        if (num == 0)
            num = callStack_[callStack_.size () - 1].var;
        if (num == 0)
            NAT_EXCEPTION (expn_, "Invalid var number", ERROR_INVALID_STRUCT_VAR_NUM_STRUCT)
        return reinterpret_cast <int8_t*> (vars_[num - 1].var) + int32_t (arg);
    }

    void ComplexCall (long long arg)
    {
        int arg_nested_call = 0;
        if (int (arg >> 32) == 0) arg_nested_call = callStack_[callStack_.size () - 1].var;
        else arg_nested_call = int (arg >> 32);
        callStack_.push ({ static_cast <int32_t> (run_line_), arg_nested_call});
        run_line_ = int (arg);
    }

    ~RunInstanceDataHandler_t ();

    void* GetVarPt (char flag, long long arg)
    {
        if ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR)        return vars_[arg].var;
        if ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR_MEMBER) return MemberVarPt (arg);
        return nullptr;
    }

    long long GetVarType (char flag, long long arg)
    {
        if ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR)        return vars_[arg].code;
        if ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR_MEMBER) return MemberVarType (arg);
        return 0;
    }

    size_t GetVarSize (char flag, long long arg)
    {
        if ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR)        return typeSizes_[vars_[arg].code];
        if ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR_MEMBER) return typeSizes_[MemberVarType (arg)];
        return 0;
    }

    size_t GetSize (char flag, long long arg)
    {
        if (isVar (flag)) return GetVarSize (flag, arg);
        if (isReg (flag)) return GetReg (arg) .size;
        if (isStr (flag)) return sizeof (char*);
        if (isNum (flag)) return sizeof (int);
        return 0;
    }

    inline bool isNum  (char flag) {return ((flag & char (~ARG_UNREF_MASK)) == ARG_NUM  ? true : false);}
    inline bool isNull (char flag) {return ((flag & char (~ARG_UNREF_MASK)) == ARG_NULL ? true : false);}
    inline bool isVar  (char flag) {return ((flag & char (~ARG_UNREF_MASK)) == ARG_VAR ||
                                            (flag & char (~ARG_UNREF_MASK)) == ARG_VAR_MEMBER ? true : false);}
    inline bool isReg  (char flag) {return ((flag & char (~ARG_UNREF_MASK)) == ARG_REG  ? true : false);}
    inline bool isData (char flag) {return isVar (flag) || isReg (flag);}
    inline bool isStr  (char flag) {return ((flag & char (~ARG_UNREF_MASK)) == ARG_STR  ? true : false);}

    void* GetPtr (char flag, long long arg)
    {
        //ErrorPrintfBox ("GetPtr\n");
        void* ret = nullptr;
        if (isVar (flag) && (ret = GetVarPt (flag, arg))) {/*ErrorPrintfBox ("GetPtr Var\n");*/ return ret;}
        if (isReg (flag)) {/*ErrorPrintfBox ("GetPtr Reg\n");*/ return GetReg (arg) .reg;}
        //ErrorPrintfBox ("GetPtr 0\n");
        return ret;
    }

    long long RspAdd ()
    {
        long long result = 0;
        for (int i = stackDumpPoint_; i < dataStack_.size (); i++)
        {
            if (dataStack_[i].size == 8) result += 8;
            else result += 4;
        }
        return result;
    }

};

RunInstanceDataHandler_t::RunInstanceDataHandler_t (std::string filename,
                                                    exception_data* expn,
                                                    std::map<std::string, UserFunc_t> importUserFuncs) :
    ScriptLoader_t    (filename, expn),
    dataStack_        (expn_),
    callStack_        (expn_),
    registers_        (),
    run_line_         (0),
    cmpr_flag_        (FLAG_NOT_SET),
    callImportFuncs_  (),
    stackDumpPoint_   (),
    func_offsets_     (new int64_t [funcs_.size()])
{
    STL_LOOP (i, vars_) i->Free ();
    for (size_t i = 0; i < userFuncs_.size (); i++)
    {
        auto found = importUserFuncs.begin ();
        if ((found = importUserFuncs.find (userFuncs_[i])) == importUserFuncs.end ())
        {
            std::string message ("function \"");
            message += userFuncs_[i];
            message += "\" has not been registered";
            NAT_EXCEPTION (expn_, message.c_str (), ERROR_NO_FUNC_IMPORT);
        }
        else
        {
            callImportFuncs_[i] = found->second;
        }

    }
}

long long RunInstanceDataHandler_t::GetVal (char arg_flag, long long arg_arg)
{
    switch (arg_flag & ~char (ARG_UNREF_MASK))
    {
        case ARG_VAR:
            return GetVar (arg_arg);
        case ARG_VAR_MEMBER:
            return GetMemberVar (arg_arg);
        case ARG_REG:
            return GetReg (arg_arg) .Get8byte ();
            break;
        case ARG_STR:
            return reinterpret_cast <int64_t> (strings_[arg_arg]);
        case ARG_NUM:
            return arg_arg;
        break;
        default:
            return 0;
    }
    return 0;
}

bool RunInstanceDataHandler_t::SetVal (char arg_flag, long long arg_arg, long long setting)
{
    switch (arg_flag & ~char (ARG_UNREF_MASK))
    {
        case ARG_VAR:
            SetVar (arg_arg, setting);
            break;
        case ARG_VAR_MEMBER:
            SetMemberVar (arg_arg, setting);
            break;
        case ARG_REG:
            GetReg (arg_arg) .Set (setting);
            break;
        default:
            return false;
    }
    return true;
}

RunInstanceDataHandler_t::~RunInstanceDataHandler_t ()
{
    delete [] func_offsets_;
}

