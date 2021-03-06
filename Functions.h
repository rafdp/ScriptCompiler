#define _ ,

#define $ instance_->

#define FUNC_OPEN  (
#define FUNC_CLOSE )

#define FUNCTION_BEGIN(name, n_params1, n_params2, params) \
void VirtualProcessor_t::name () \
{ \
Arg_t& arg = $ args_[instance_->run_line_]; \
ManageInputArgs_t man (ExpectedArg_t (n_params1, n_params2, params), arg); \
if (man.error) {currentReturn_ = man.CreateError (); return;} \

#define FUNCTION_END \
currentReturn_ = RET_NO_ERRORS; \
}

#define ARG_SIZE_SWITCH_PTR_LONG(num, offset, preEmission, postEmission) \
    switch ($ GetSize (arg.flag##num, arg.arg##num)) \
    { \
        case sizeof (int8_t): \
            preEmission reinterpret_cast<int8_t*> ($ GetPtr (arg.flag##num, arg.arg##num)) + offset postEmission; \
            break; \
        case sizeof (int16_t): \
            preEmission reinterpret_cast<int16_t*> ($ GetPtr (arg.flag##num, arg.arg##num)) + offset postEmission; \
            break; \
        case sizeof (int32_t): \
            preEmission reinterpret_cast<int32_t*> ($ GetPtr (arg.flag##num, arg.arg##num)) + offset postEmission; \
            break; \
        case sizeof (int64_t): \
            preEmission reinterpret_cast<int64_t*> ($ GetPtr (arg.flag##num, arg.arg##num)) + offset postEmission; \
            break; \
        default: \
            break;\
    }


FUNCTION_BEGIN (RebuildVar, 2, 4, ARG_VAR _ ARG_NULL _ ARG_NUM)
    VarData_t& var = $ vars_[arg.arg1];
    var.Delete ();
    if ($ isNum (arg.flag2))
        var.code = arg.arg2;
    else
        var.code = TYPE_QWORD;
    var.size = $ typeSizes_[var.code];
    var.Free ();
FUNCTION_END

FUNCTION_BEGIN (Push, 5, 0, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)

    //ErrorPrintfBox ("%d %d %lld %lld", arg.flag1, arg.flag2, arg.arg1, arg.arg2);
    $ dataStack_.push (StackData_t ($ GetVal (arg.flag1, arg.arg1), $ GetSize (arg.flag1, arg.arg1)));
FUNCTION_END

FUNCTION_BEGIN (Pop, 4, 0, ARG_NULL _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
    int64_t val = $ dataStack_.pop ().data;
    if (! $ isNull (arg.flag1)) $ SetVal (arg.flag1, arg.arg1, val);
FUNCTION_END

FUNCTION_BEGIN (Mov, 3, 5, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)

    if ($ isVar (arg.flag1) && $ isVar (arg.flag2) &&
        $ GetVarType (arg.flag1, arg.arg1) == $ GetVarType (arg.flag2, arg.arg2))
        memcpy ($ GetVarPt (arg.flag1, arg.arg1),
                $ GetVarPt (arg.flag2, arg.arg2),
                $ GetVarSize (arg.flag2, arg.arg2));
    else
    {
        /*ErrorPrintfBox ("MOV before\n%s %I64d %I64d\n%s %I64d %I64d\n",
                        ARG_D[arg.flag1].c_str (), arg.arg1, $ GetVal (arg.flag1, arg.arg1),
                        ARG_D[arg.flag2].c_str (), arg.arg2, $ GetVal (arg.flag2, arg.arg2));
        int64_t res = $ GetVal (arg.flag2, arg.arg2);*/
        $ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag2, arg.arg2));

        /*ErrorPrintfBox ("MOV after\n%s %I64d %I64d\n%s %I64d %I64d\n",
                        ARG_D[arg.flag1].c_str (), arg.arg1, $ GetVal (arg.flag1, arg.arg1),
                        ARG_D[arg.flag2].c_str (), arg.arg2, $ GetVal (arg.flag2, arg.arg2));
        */
    }

FUNCTION_END

FUNCTION_BEGIN (Lea, 2, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
    if ($ GetVarType (arg.flag1, arg.arg1) != TYPE_PTR)
        {currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, "arg1 is not a pointer"); return;}
    if ($ isVar (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, reinterpret_cast<int64_t> ($ GetVarPt (arg.flag2, arg.arg2)));
    else
    if ($ isReg (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, reinterpret_cast<int64_t> ($ GetReg (arg.arg2).reg));
    else
    if ($ isStr (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, static_cast<int64_t> ($ GetVal (arg.flag2, arg.arg2)));
FUNCTION_END

FUNCTION_BEGIN (Print, 1, 5, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)
    //ErrorPrintfBox ("Called Print\n");
    const char error0[] = "nullptr pointer, cannot print str";
    const char error1[] = "Invalid cmd";

    #define _RET_ERROR_(n) {currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, error##n); return;}

    #define PRINT_FORMATTED(val) \
    {\
    switch (arg.arg1) \
        { \
            case PRINT_STRING: \
                if (!val) _RET_ERROR_ (0) \
                printf ("%s", reinterpret_cast <int8_t*> (val)); \
                break; \
            case PRINT_NUMBER: \
                printf ("%I64d", reinterpret_cast <int64_t> (val)); \
                break; \
            default: \
                break; \
        } \
    }

    if ($ isData (arg.flag2))
        PRINT_FORMATTED ($ GetVal (arg.flag2, arg.arg2))
    else
    if ($ isStr (arg.flag2))
        PRINT_FORMATTED ( ($ strings_[arg.arg2]))
    else
    if ($ isNum (arg.flag2) && arg.arg2 == PRINT_STACK)
        PRINT_FORMATTED ($ dataStack_.top() . data)
    else _RET_ERROR_ (1)

    #undef _RET_ERROR_
FUNCTION_END

FUNCTION_BEGIN (Cmpr, 4, 4, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)

    int64_t val1 = $ GetVal (arg.flag1, arg.arg1),
              val2 = $ GetVal (arg.flag2, arg.arg2);

    if (val1 > val2)
        $ cmpr_flag_ = FLAG_HIGH;
    if (val1 < val2)
        $ cmpr_flag_ = FLAG_LOW;
    if (val1 == val2)
        $ cmpr_flag_ = FLAG_EQUAL;

FUNCTION_END

FUNCTION_BEGIN (Jmp, 1, 0, ARG_LABEL)
    $ run_line_ = arg.arg1;
FUNCTION_END

#define JUMP_AUTOFILL(name, situation) \
FUNCTION_BEGIN (name, 1, 0, ARG_LABEL) \
const char error1[] = "Flag not set"; \
if ($ cmpr_flag_ == FLAG_NOT_SET) \
currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, error1); \
if (situation) \
$ run_line_ = arg.arg1; \
FUNCTION_END

JUMP_AUTOFILL (Je, $ cmpr_flag_ == FLAG_EQUAL)
JUMP_AUTOFILL (Jne, $ cmpr_flag_ != FLAG_EQUAL)
JUMP_AUTOFILL (Ja, $ cmpr_flag_ == FLAG_HIGH)
JUMP_AUTOFILL (Jb, $ cmpr_flag_ == FLAG_LOW)
JUMP_AUTOFILL (Jae, $ cmpr_flag_ == FLAG_HIGH || $ cmpr_flag_ == FLAG_EQUAL)
JUMP_AUTOFILL (Jbe, $ cmpr_flag_ == FLAG_LOW || $ cmpr_flag_ == FLAG_EQUAL)

#undef JUMP_AUTOFILL


FUNCTION_BEGIN (Ret, 1, 0, ARG_NULL)
    //ErrorPrintfBox ("Returning to line %d", $ callStack_.top ().retLine);
    $ run_line_ = $ callStack_.top ().retLine + 1;
    $ callStack_.pop ();
    //$ callStack_.Dump ();
    //ErrorPrintfBox ("Returned to line %d", $ run_line_);
FUNCTION_END

FUNCTION_BEGIN (Call, 2, 0, ARG_FUNC _ ARG_FUNC_MEMBER)
        //ErrorPrintfBox ("%I64d", $ run_line_);
    if (arg.flag1 == ARG_FUNC_MEMBER)
        $ ComplexCall (arg.arg1);
    else
    {
        $ callStack_.push (CallInfo_t (static_cast <int32_t> ($ run_line_)));
        //$ callStack_.Dump ();
        //ErrorPrintfBox ("Call from line %d", $ run_line_);
        $ run_line_ = arg.arg1;
    }
FUNCTION_END

FUNCTION_BEGIN (Decr, 3, 0, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
    $ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag1, arg.arg1) - 1);
FUNCTION_END

FUNCTION_BEGIN (InitStackDumpPoint, 1, 0, ARG_NULL)
    $ stackDumpPoint_ = $ dataStack_.size ();
FUNCTION_END

FUNCTION_BEGIN (JIT_Printf, 1, 0, ARG_NULL)
    /*std::string code;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++)
        code += "push " + GetAsmNumString (*i) + "\n";
    code += "mov eax, " + GetAsmNumString (int64_t (printf)) + "\ncall eax\n";
    code += "add esp, " + GetAsmNumString ( ($ dataStack_.size ()) * 4, "") + "\nretn\n";
    //RunAsm (code);*/
FUNCTION_END

#define ASM_FILL_STACK(str) \
for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++) \
PushStackValueString (*i, &str);/*str += "push " + GetAsmNumString (*i) + "\n"; */

#define ASM_CALL_FUNC(ptr, str) \
str += "mov eax, " + GetAsmNumString (int (ptr)) + "\ncall eax\n";

FUNCTION_BEGIN (JIT_Call_Void, 1, 0, ARG_DLL_FUNC)
    //ErrorPrintfBox ("JIT_Call_Void %d %d %s\n", $ stackDumpPoint_, $ dataStack_.size (), __PRETTY_FUNCTION__);

    JitCompiler_t comp;
    //comp.int3 ();
    PushStackValueJit (& $ dataStack_, $ stackDumpPoint_, &comp, expn_);
    //comp.int3 ();
    comp.mov<int64_t> (comp.r_rax, reinterpret_cast<int64_t> ($ dllResolved_[arg.arg1]));
    comp.call<int64_t> (comp.r_rax);
    comp.add<int64_t> (comp.r_rsp, $ RspAdd ());
    comp.retn ();
    comp.BuildAndRun ();

FUNCTION_END

FUNCTION_BEGIN (JIT_Call_DWord, 1, 3, ARG_DLL_FUNC _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER)
    if ($ GetSize (arg.flag2, arg.arg2) != sizeof (DWORD))
        currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, $ isReg (arg.flag2) ? "Invalid reg size" : "Invalid var size");
    JitCompiler_t comp;

    PushStackValueJit (& $ dataStack_, $ stackDumpPoint_, &comp, expn_);
    comp.mov<int64_t> (comp.r_rax, reinterpret_cast<int64_t> ($ dllResolved_[arg.arg1]));
    comp.call<int64_t> (comp.r_rax);

    ARG_SIZE_SWITCH_PTR_LONG(2, 0, comp.mov FUNC_OPEN, _ comp.r_rax FUNC_CLOSE)

    //comp.add<int64_t> (comp.r_rsp, $ RspAdd ());
    comp.retn ();
    comp.BuildAndRun ();
FUNCTION_END

FUNCTION_BEGIN (JIT_Call_QWord, 1, 3, ARG_DLL_FUNC _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER)
    if ($ GetSize (arg.flag2, arg.arg2) != sizeof (QWORD))
        currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, $ isReg (arg.flag2) ? "Invalid reg size" : "Invalid var size");
    JitCompiler_t comp;
    PushStackValueJit (& $ dataStack_, $ stackDumpPoint_, &comp, expn_);
    comp.mov<int64_t> (comp.r_rax, reinterpret_cast<int64_t> ($ dllResolved_[arg.arg1]));
    comp.call<int64_t> (comp.r_rax);

    ARG_SIZE_SWITCH_PTR_LONG(2, 0, comp.mov FUNC_OPEN, _ comp.r_rax FUNC_CLOSE)

    //comp.add<int64_t> (comp.r_rsp, $ RspAdd ());
    comp.retn ();
    comp.BuildAndRun ();
FUNCTION_END

#undef ASM_FILL_STACK
#undef ASM_CALL_FUNC
#define ARITHMETIC_FUNCTION(name, operator, check0) \
FUNCTION_BEGIN(name, 3, 4, ARG_VAR _ ARG_REG _ ARG_VAR_MEMBER _ ARG_VAR _ ARG_REG _ ARG_NUM _ ARG_VAR_MEMBER) \
long long opVal = $ GetVal (arg.flag2, arg.arg2); \
if (check0 && opVal == 0) \
{ \
    currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)"); \
    return; \
} \
$ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag1, arg.arg1) operator opVal); \
FUNCTION_END

ARITHMETIC_FUNCTION (Add, +, false)
ARITHMETIC_FUNCTION (Sub, -, false)
ARITHMETIC_FUNCTION (Mul, *, false)
ARITHMETIC_FUNCTION (Div, /, true)
#undef ARITHMETIC_FUNCTION

#define STACK_ARITHMETIC_FUNCTION(name, operator, check0) \
FUNCTION_BEGIN(name, 0, 0, ARG_NULL) \
long long val1 = $ dataStack_.pop(); \
long long val0 = $ dataStack_.pop(); \
if (check0 && val1 == 0) \
{ \
    currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)"); \
    return; \
} \
$ dataStack_.push(StackData_t (val0 operator val1, sizeof (long long))); \
FUNCTION_END


STACK_ARITHMETIC_FUNCTION (Adds, +, false)
STACK_ARITHMETIC_FUNCTION (Subs, -, false)
STACK_ARITHMETIC_FUNCTION (Muls, *, false)
STACK_ARITHMETIC_FUNCTION (Divs, /, true)
#undef STACK_ARITHMETIC_FUNCTION

FUNCTION_BEGIN (Sqrt, 3, 0, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
    int64_t val = $ GetVal (arg.flag1, arg.arg1);

    if (val < 0)
        currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, "Cannot calculate square root of a negative value");

    $ SetVal (arg.flag1, arg.arg1, static_cast<int64_t> (round (sqrt (static_cast <double> (val)))));
FUNCTION_END

#undef FUNCTION_BEGIN

#define FUNCTION_BEGIN(name, n_params1, n_params2, params) \
void VirtualProcessor_t::Jit_##name (JitCompiler_t* comp, int i) \
{ \
Arg_t& arg = $ args_[i]; \
ManageInputArgs_t man (ExpectedArg_t (n_params1, n_params2, params), arg); \

#define CALL_MEMBER(name, params) \
    comp->mov<int64_t>  (comp->r_rax, reinterpret_cast<int64_t> (reinterpret_cast<void*>(&name))); \
    comp->ParameterPush (params); \
    comp->call<int64_t> (comp->r_rax); \
    comp->add<int64_t>  (comp->r_rsp, 0x30/*8*sizeof (int64_t)*/);

#define DEFAULT_BODY(name) \
    CALL_MEMBER(VirtualProcessor_t::name, reinterpret_cast<int64_t> (reinterpret_cast<void*>(this))) \
    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_)); \
    comp->inc<int64_t> (&comp->r_rax);

FUNCTION_BEGIN (RebuildVar, 2, 4, ARG_VAR _ ARG_NULL _ ARG_NUM)
DEFAULT_BODY (RebuildVar)
FUNCTION_END

FUNCTION_BEGIN (Push, 5, 0, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
DEFAULT_BODY (Push)
FUNCTION_END

FUNCTION_BEGIN (Pop, 4, 0, ARG_NULL _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
DEFAULT_BODY (Pop)
FUNCTION_END

FUNCTION_BEGIN (Mov, 3, 5, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)

size_t size1 = $ GetSize (arg.flag1, arg.arg1);
size_t size2 = $ GetSize (arg.flag2, arg.arg2);

if ($ isNum (arg.flag2) || $ isStr (arg.flag2))
{
    int64_t argN = $ isNum (arg.flag2) ? arg.arg2 : reinterpret_cast<int64_t> ($ strings_[arg.arg2]);

    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)
/*
    DebugBreak ();
    //TestFunc (instance_);
    //TestFunc (reinterpret_cast<void*>(&RunInstanceDataHandler_t::GetPtr));
    void* ptr = $ GetPtr (ARG_REG, REG_AQX);
    ErrorPrintfBox ("%I64X", ptr);
    DebugBreak ();
*/
    switch (size1)
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (&comp->r_rax, static_cast<int8_t> (argN));
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (&comp->r_rax, static_cast<int16_t> (argN));
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (&comp->r_rax, static_cast<int32_t> (argN));
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (&comp->r_rax, static_cast<int64_t> (argN));
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 1 size");
            return;
    }
}
else
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)
    comp->push<int64_t> (comp->r_rax);
    comp->push<int64_t> (0);

    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag2 _ arg.arg2)

    comp->pop<int64_t> (comp->r_rcx);
    comp->pop<int64_t> (comp->r_rcx);

    if (size1 >= size2)
    {
        switch (size1)
        {
            case sizeof (int8_t):
                comp->mov<int8_t> (&comp->r_rcx, 0);
                break;
            case sizeof (int16_t):
                comp->mov<int16_t> (&comp->r_rcx, 0);
                break;
            case sizeof (int32_t):
                comp->mov<int32_t> (&comp->r_rcx, 0);
                break;
            case sizeof (int64_t):
                comp->mov<int64_t> (&comp->r_rcx, 0);
                break;
            default:
                currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 1 size");
                return;
        }
    }

    comp->mov<int64_t> (comp->r_rax, &comp->r_rax);

    switch (MIN (size1, size2))
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (&comp->r_rcx, comp->r_rax);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument size");
            return;
    }
}
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Print, 1, 5, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)
DEFAULT_BODY (Print)
FUNCTION_END

FUNCTION_BEGIN (Cmpr, 4, 4, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
size_t size1 = $ GetSize (arg.flag1, arg.arg1);
size_t size2 = $ GetSize (arg.flag2, arg.arg2);

if ($ isNum (arg.flag2) && ! $ isNum (arg.flag1))
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)


    size_t jg_offset = 0;
    size_t jl_offset = 0;
    size_t jmp_offset = 0;


    comp->mov (comp->r_rcx, static_cast<int64_t> (0));

    switch (size1)
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (comp->r_rcx, &comp->r_rax);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (comp->r_rcx, &comp->r_rax);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (comp->r_rcx, &comp->r_rax);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rcx, &comp->r_rax);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 1 size");
            return;
    }

    comp->cmp<int64_t> (comp->r_rcx, arg.arg2);

    comp->jg (0);
    jg_offset = comp->Size() - sizeof (int32_t);

    comp->jl (0);
    jl_offset = comp->Size() - sizeof (int32_t);

    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_EQUAL));

    comp->jmp (0);
    jmp_offset = comp->Size() - sizeof (int32_t);

    JmpPatchRequestOffset (jg_offset, comp->Size() + 1);
    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_HIGH));

    comp->jmp (0);
    jg_offset = comp->Size() - sizeof (int32_t);

    JmpPatchRequestOffset (jl_offset, comp->Size() + 1);

    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_LOW));

    //! finally:
    JmpPatchRequestOffset (jg_offset, comp->Size() + 1);
    JmpPatchRequestOffset (jmp_offset, comp->Size() + 1);


}
else
if ($ isNum (arg.flag1) && ! $ isNum (arg.flag2))
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag2 _ arg.arg2)


    size_t jg_offset = 0;
    size_t jl_offset = 0;
    size_t jmp_offset = 0;


    comp->mov<int64_t> (comp->r_rcx, static_cast<int64_t> (0));

    switch (size2)
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (comp->r_rcx, &comp->r_rax);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (comp->r_rcx, &comp->r_rax);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (comp->r_rcx, &comp->r_rax);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rcx, &comp->r_rax);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 2 size");
            return;
    }

    comp->cmp<int64_t> (comp->r_rcx, arg.arg2);

    comp->jg (0);
    jg_offset = comp->Size() - sizeof (int32_t);

    comp->jl (0);
    jl_offset = comp->Size() - sizeof (int32_t);

    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_EQUAL));

    comp->jmp (0);
    jmp_offset = comp->Size() - sizeof (int32_t);

    JmpPatchRequestOffset (jg_offset, comp->Size() + 1);
    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_HIGH));

    comp->jmp (0);
    jg_offset = comp->Size() - sizeof (int32_t);

    JmpPatchRequestOffset (jl_offset, comp->Size() + 1);

    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_LOW));

    //! finally:
    JmpPatchRequestOffset (jg_offset, comp->Size() + 1);
    JmpPatchRequestOffset (jmp_offset, comp->Size() + 1);


}
else
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)
    comp->push<int64_t> (comp->r_rax);
    comp->push<int64_t> (0);

    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag2 _ arg.arg2)

    comp->pop<int64_t> (comp->r_rcx);
    comp->pop<int64_t> (comp->r_rcx);


    size_t jg_offset = 0;
    size_t jl_offset = 0;
    size_t jmp_offset = 0;

    switch (size1)
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (comp->r_rcx, &comp->r_rcx);
            comp->push<int8_t> (comp->r_rcx);
            comp->mov<int64_t> (comp->r_rcx, static_cast<int64_t> (0));
            comp->pop<int8_t> (comp->r_rcx);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (comp->r_rcx, &comp->r_rcx);
            comp->push<int16_t> (comp->r_rcx);
            comp->mov<int64_t> (comp->r_rcx, static_cast<int64_t> (0));
            comp->pop<int16_t> (comp->r_rcx);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (comp->r_rcx, &comp->r_rcx);
            comp->push<int32_t> (comp->r_rcx);
            comp->mov<int64_t> (comp->r_rcx, static_cast<int64_t> (0));
            comp->pop<int32_t> (comp->r_rcx);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rcx, &comp->r_rcx);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 1 size");
            return;
    }

    switch (size2)
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (comp->r_rax, &comp->r_rax);
            comp->push<int8_t> (comp->r_rax);
            comp->mov<int64_t> (comp->r_rax, static_cast<int64_t> (0));
            comp->pop<int8_t> (comp->r_rax);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (comp->r_rax, &comp->r_rax);
            comp->push<int16_t> (comp->r_rax);
            comp->mov<int64_t> (comp->r_rax, static_cast<int64_t> (0));
            comp->pop<int16_t> (comp->r_rax);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (comp->r_rax, &comp->r_rax);
            comp->push<int32_t> (comp->r_rax);
            comp->mov<int64_t> (comp->r_rax, static_cast<int64_t> (0));
            comp->pop<int32_t> (comp->r_rax);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rax, &comp->r_rax);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 2 size");
            return;
    }

    comp->cmp<int64_t> (comp->r_rcx, comp->r_rcx);

    comp->jg (0);
    jg_offset = comp->Size() - sizeof (int32_t);

    comp->jl (0);
    jl_offset = comp->Size() - sizeof (int32_t);

    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_EQUAL));

    comp->jmp (0);
    jmp_offset = comp->Size() - sizeof (int32_t);

    JmpPatchRequestOffset (jg_offset, comp->Size() + 1);
    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_HIGH));

    comp->jmp (0);
    jg_offset = comp->Size() - sizeof (int32_t);

    JmpPatchRequestOffset (jl_offset, comp->Size() + 1);

    comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
    comp->mov<int8_t>  (&comp->r_rax, static_cast<int8_t> (FLAG_LOW));

    //! finally:
    JmpPatchRequestOffset (jg_offset, comp->Size() + 1);
    JmpPatchRequestOffset (jmp_offset, comp->Size() + 1);
}
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Jmp, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Jmp,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))
comp->jmp (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
FUNCTION_END

FUNCTION_BEGIN (Jne, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Jne,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))

comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
comp->cmp<int8_t>  (&comp->r_rax, '\0');
comp->jne (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Je, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Je,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))

comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
comp->cmp<int8_t>  (&comp->r_rax, '\0');
comp->je (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Ja, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Ja,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))

comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
comp->cmp<int8_t>  (&comp->r_rax, '\0');
comp->jg (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Jae, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Jae,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))

comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
comp->cmp<int8_t>  (&comp->r_rax, '\0');
comp->jge (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Jb, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Jb,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))

comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
comp->cmp<int8_t>  (&comp->r_rax, '\0');
comp->jl (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Jbe, 1, 0, ARG_LABEL)
CALL_MEMBER (VirtualProcessor_t::Jbe,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))

comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->cmpr_flag_));
comp->cmp<int8_t>  (&comp->r_rax, '\0');
comp->jle (0);
JmpPatchRequestLine (static_cast <int64_t> (comp->Size() - sizeof (int32_t)), arg.arg1);
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Ret, 1, 0, ARG_NULL)
CALL_MEMBER (VirtualProcessor_t::Ret,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))
comp->retn ();
FUNCTION_END

FUNCTION_BEGIN (Call, 2, 0, ARG_FUNC _ ARG_FUNC_MEMBER)
CALL_MEMBER (VirtualProcessor_t::Call,
             reinterpret_cast<int64_t> (reinterpret_cast<void*>(this)))
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->callr (0);
JmpPatchRequestLine (static_cast<int64_t> (comp->Size() - sizeof (int32_t)), static_cast <int64_t> (static_cast <int32_t> (arg.arg1) + 1));
comp->pop<int64_t> (comp->r_rax);
FUNCTION_END

FUNCTION_BEGIN (Decr, 3, 0, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
DEFAULT_BODY (Decr)
FUNCTION_END

FUNCTION_BEGIN (InitStackDumpPoint, 1, 0, ARG_NULL)
DEFAULT_BODY (InitStackDumpPoint)
FUNCTION_END

FUNCTION_BEGIN (JIT_Printf, 1, 0, ARG_NULL)
DEFAULT_BODY (JIT_Printf)
FUNCTION_END

FUNCTION_BEGIN (JIT_Call_Void, 1, 0, ARG_DLL_FUNC)
//comp->int3 ();
DEFAULT_BODY (JIT_Call_Void)
FUNCTION_END

FUNCTION_BEGIN (JIT_Call_DWord, 1, 0, ARG_DLL_FUNC)
DEFAULT_BODY (JIT_Call_DWord)
FUNCTION_END

FUNCTION_BEGIN (JIT_Call_QWord, 1, 0, ARG_DLL_FUNC)
DEFAULT_BODY (JIT_Call_QWord)
FUNCTION_END

FUNCTION_BEGIN (Add, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)

size_t size1 = $ GetSize (arg.flag1, arg.arg1);
size_t size2 = $ GetSize (arg.flag2, arg.arg2);

if ($ isNum (arg.flag2))
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)

    switch (size1)
    {
        case sizeof (int8_t):
            comp->add<int8_t> (&comp->r_rax,  static_cast<int8_t> (arg.arg2));
            break;
        case sizeof (int16_t):
            comp->add<int16_t> (&comp->r_rax, static_cast<int16_t> (arg.arg2));
            break;
        case sizeof (int32_t):
            comp->add<int32_t> (&comp->r_rax, static_cast<int32_t> (arg.arg2));
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rbx, static_cast<int64_t> (arg.arg2));
            comp->add<int64_t> (&comp->r_rax, comp->r_rbx);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 1 size");
            return;
    }
}
else
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)
    comp->push<int64_t> (comp->r_rax);
    comp->push<int64_t> (0);

    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag2 _ arg.arg2)

    comp->pop<int64_t> (comp->r_rcx);
    comp->pop<int64_t> (comp->r_rcx);

    switch (MIN (size1, size2))
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (comp->r_rax, &comp->r_rax);
            comp->add<int8_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (comp->r_rax, &comp->r_rax);
            comp->add<int16_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (comp->r_rax, &comp->r_rax);
            comp->add<int32_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rax, &comp->r_rax);
            comp->add<int64_t> (&comp->r_rcx, comp->r_rax);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument size");
            return;
    }

}
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);

FUNCTION_END

FUNCTION_BEGIN (Sub, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)

size_t size1 = $ GetSize (arg.flag1, arg.arg1);
size_t size2 = $ GetSize (arg.flag2, arg.arg2);

if ($ isNum (arg.flag2))
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)

    switch (size1)
    {
        case sizeof (int8_t):
            comp->sub<int8_t> (&comp->r_rax,  static_cast<int8_t> (arg.arg2));
            break;
        case sizeof (int16_t):
            comp->sub<int16_t> (&comp->r_rax, static_cast<int16_t> (arg.arg2));
            break;
        case sizeof (int32_t):
            comp->sub<int32_t> (&comp->r_rax, static_cast<int32_t> (arg.arg2));
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rbx, static_cast<int64_t> (arg.arg2));
            comp->sub<int64_t> (&comp->r_rax, comp->r_rbx);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument 1 size");
            return;
    }
}
else
{
    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag1 _ arg.arg1)
    comp->push<int64_t> (comp->r_rax);
    comp->push<int64_t> (0);

    CALL_MEMBER (RunInstanceDataHandler_t::GetPtr,
                 reinterpret_cast<int64_t> (instance_) _ arg.flag2 _ arg.arg2)

    comp->pop<int64_t> (comp->r_rcx);
    comp->pop<int64_t> (comp->r_rcx);

    switch (MIN (size1, size2))
    {
        case sizeof (int8_t):
            comp->mov<int8_t> (comp->r_rax, &comp->r_rax);
            comp->sub<int8_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int16_t):
            comp->mov<int16_t> (comp->r_rax, &comp->r_rax);
            comp->sub<int16_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int32_t):
            comp->mov<int32_t> (comp->r_rax, &comp->r_rax);
            comp->sub<int32_t> (&comp->r_rcx, comp->r_rax);
            break;
        case sizeof (int64_t):
            comp->mov<int64_t> (comp->r_rax, &comp->r_rax);
            comp->sub<int64_t> (&comp->r_rcx, comp->r_rax);
            break;
        default:
            currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "invalid argument size");
            return;
    }
}
comp->push<int64_t> (comp->r_rax);
comp->mov<int64_t> (comp->r_rax, reinterpret_cast<int64_t> (&instance_->run_line_));
comp->inc<int64_t> (&comp->r_rax);
comp->pop<int64_t> (comp->r_rax);

FUNCTION_END

FUNCTION_BEGIN (Mul, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)
DEFAULT_BODY (Mul)
FUNCTION_END

FUNCTION_BEGIN (Div, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)
DEFAULT_BODY (Div)
FUNCTION_END

FUNCTION_BEGIN (Adds, 1, 0, ARG_NULL)
DEFAULT_BODY (Adds)
FUNCTION_END

FUNCTION_BEGIN (Subs, 1, 0, ARG_NULL)
DEFAULT_BODY (Subs)
FUNCTION_END

FUNCTION_BEGIN (Muls, 1, 0, ARG_NULL)
DEFAULT_BODY (Muls)
FUNCTION_END

FUNCTION_BEGIN (Divs, 1, 0, ARG_NULL)
DEFAULT_BODY (Muls)
FUNCTION_END

FUNCTION_BEGIN (Lea, 2, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
DEFAULT_BODY (Lea)
FUNCTION_END

FUNCTION_BEGIN (Sqrt, 3, 0, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
//comp->int3 ();
DEFAULT_BODY (Sqrt)
FUNCTION_END

#undef FUNCTION_BEGIN
#undef FUNCTION_END

#undef ARG_SIZE_SWITCH_PTR_LONG
#undef ARITHMETIC_FUNCTION_ADD_SUB_64_PART
#undef ARITHMETIC_FUNCTION_ADD_SUB_MUL_BASE


#undef $

#undef _

#undef FUNC_OPEN
#undef FUNC_CLOSE
