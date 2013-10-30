#define _ ,

#define FUNCTION_BEGIN(name, n_params1, n_params2, params) \
ErrorReturn_t VirtualProcessor_t::name (Arg_t arg) \
{ \
ManageInputArgs_t man (ExpectedArg_t (n_params1, n_params2, params), arg); \
if (man.error) return man.CreateError();

#define FUNCTION_END \
return RET_NO_ERRORS; \
}

#define CHECK_VAR_TYPE()

#define $ instance_->

FUNCTION_BEGIN(RebuildVar, 2, 4, ARG_VAR _ ARG_NULL _ ARG_NUM)
    VarData_t& var = $ vars_[arg.arg1];
    var.Delete();
    var.code = $ isNum (arg.arg2) ? arg.arg2 : TYPE_QWORD;
    var.size = $ typeSizes_[var.code];
    var.Free();
FUNCTION_END

FUNCTION_BEGIN(Push, 5, 0, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
    $ dataStack_.push (StackData_t ($ GetVal (arg.flag1, arg.arg1), $ GetSize(arg.flag1, arg.arg1)));
FUNCTION_END

FUNCTION_BEGIN(Pop, 4, 0, ARG_NULL _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
    long long val = $ dataStack_.pop ().data;
    if (! $ isNull (arg.flag1)) $ SetVal (arg.flag1, arg.arg1, val);
FUNCTION_END

FUNCTION_BEGIN(Mov, 3, 5, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)

    if ($ isVar (arg.flag1) && $ isVar (arg.flag2) &&
        $ GetVarType (arg.flag1, arg.arg1) == $ GetVarType (arg.flag2, arg.arg2))
        memcpy ($ GetVarPt (arg.flag1, arg.arg1),
                $ GetVarPt (arg.flag2, arg.arg2),
                $ GetVarSize (arg.flag2, arg.arg2));
    else
        $ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag2, arg.arg2));
FUNCTION_END

FUNCTION_BEGIN(Lea, 2, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
    if ($ GetVarType (arg.flag1, arg.arg1) != TYPE_PTR)
        return ErrorReturn_t (RET_ERROR_CONTINUE, "arg1 is not a pointer");
    if ($ isVar (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, (long long)($ GetVarPt (arg.flag2, arg.arg2)));
    else
    if ($ isReg (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, (long long)($ GetReg (arg.arg2).reg));
    else
    if ($ isStr (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, (long long)($ GetVal (arg.flag2, arg.arg2)));
FUNCTION_END

FUNCTION_BEGIN(Print, 1, 5, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)
    const char error0[] = "Null pointer, cannot print str";
    const char error1[] = "Invalid cmd";

    #define _RET_ERROR_(n) return ErrorReturn_t (RET_ERROR_CONTINUE, error##n);

    #define PRINT_FORMATTED(val) \
    {\
    switch (arg.arg1) \
        { \
            case PRINT_STRING: \
                if (!val) _RET_ERROR_(0) \
                printf ("%s", (char*)val); \
                break; \
            case PRINT_NUMBER: \
                printf ("%lld", val); \
                break; \
        } \
    }

    if ($ isData (arg.flag2))
        PRINT_FORMATTED ($ GetVal (arg.flag2, arg.arg2))
    else
    if ($ isStr (arg.flag2))
        PRINT_FORMATTED (($ strings_[arg.arg2]))
    else
    if ($ isNum (arg.flag2) && arg.arg2 == PRINT_STACK)
        PRINT_FORMATTED ($ dataStack_.top())
    else _RET_ERROR_(1)

    #undef _RET_ERROR_
FUNCTION_END

FUNCTION_BEGIN(Cmpr, 4, 4, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)

    long long val1 = $ GetVal (arg.flag1, arg.arg1),
              val2 = $ GetVal (arg.flag2, arg.arg2);

    if (val1 > val2)
        $ cmpr_flag_ = FLAG_HIGH;
    if (val1 < val2)
        $ cmpr_flag_ = FLAG_LOW;
    if (val1 == val2)
        $ cmpr_flag_ = FLAG_EQUAL;

FUNCTION_END

FUNCTION_BEGIN(Jmp, 1, 0, ARG_LABEL)
    $ run_line_ = $ labels_[arg.arg1];
FUNCTION_END

#define JUMP_AUTOFILL(name, situation) \
FUNCTION_BEGIN(name, 1, 0, ARG_LABEL) \
const char error1[] = "Flag not set"; \
if ($ cmpr_flag_ == FLAG_NOT_SET) \
return ErrorReturn_t (RET_ERROR_CONTINUE, error1); \
if (situation) \
$ run_line_ = $ labels_[arg.arg1]; \
FUNCTION_END

JUMP_AUTOFILL (Je, $ cmpr_flag_ == FLAG_EQUAL)
JUMP_AUTOFILL (Jne, $ cmpr_flag_ != FLAG_EQUAL)
JUMP_AUTOFILL (Ja, $ cmpr_flag_ == FLAG_HIGH)
JUMP_AUTOFILL (Jb, $ cmpr_flag_ == FLAG_LOW)
JUMP_AUTOFILL (Jae, $ cmpr_flag_ == FLAG_HIGH || $ cmpr_flag_ == FLAG_EQUAL)
JUMP_AUTOFILL (Jbe, $ cmpr_flag_ == FLAG_LOW || $ cmpr_flag_ == FLAG_EQUAL)

#undef JUMP_AUTOFILL


FUNCTION_BEGIN(Ret, 1, 0, ARG_NULL)
    $ run_line_ = $ callStack_.pop().retLine;
FUNCTION_END

FUNCTION_BEGIN(Call, 2, 0, ARG_FUNC _ ARG_FUNC_MEMBER)
    if (arg.flag1 == ARG_FUNC_MEMBER)
        $ ComplexCall (arg.arg1);
    else
    {
        $ callStack_.push (CallInfo_t($ run_line_));
        $ run_line_ = arg.arg1;
    }
FUNCTION_END

FUNCTION_BEGIN(Decr, 1, 0, ARG_VAR _ ARG_VAR_MEMBER)
    $ SetVal(arg.flag1, arg.arg1, $ GetVal(arg.flag1, arg.arg1) - 1);
FUNCTION_END

FUNCTION_BEGIN(InitStackDumpPoint, 1, 0, ARG_NULL)
    $ stackDumpPoint_ = $ dataStack_.size();
FUNCTION_END

FUNCTION_BEGIN(JIT_Printf, 1, 0, ARG_NULL)
    std::string code;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end(); i++)
        code += "push " + GetAsmNumString (*i) + "\n";
    code += "mov eax, " + GetAsmNumString (int(printf)) + "\ncall eax\n";
    code += "add esp, " + GetAsmNumString (($ dataStack_.size()) * 4, "") + "\nretn\n";
    //RunAsm (code);
FUNCTION_END

#define ASM_FILL_STACK(str) \
for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end(); i++) \
PushStackValueString (*i, &str);/*str += "push " + GetAsmNumString (*i) + "\n"; */

#define ASM_CALL_FUNC(ptr, str) \
str += "mov eax, " + GetAsmNumString (int(ptr)) + "\ncall eax\n";

FUNCTION_BEGIN(JIT_Call_Void, 1, 0, ARG_DLL_FUNC)
    //ErrorPrintfBox ("%d %d %s\n", $ stackDumpPoint_, $ dataStack_.size(), __PRETTY_FUNCTION__);

    JitCompiler_t comp;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end(); i++)
        PushStackValueJit (*i, &comp);
    comp.mov (comp.r_rax, int32_t($ dllResolved_[arg.arg1]));
    comp.call (comp.r_rax);
    comp.add(comp.r_rsp, $ RspAdd());
    comp.retn();
    comp.BuildAndRun();

FUNCTION_END

FUNCTION_BEGIN(JIT_Call_DWord, 1, 3, ARG_DLL_FUNC _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER)
    if ($ GetSize(arg.flag2, arg.arg2) != sizeof (DWORD))
        return ErrorReturn_t (RET_ERROR_CONTINUE, $ isReg (arg.flag2) ? "Invalid reg size" : "Invalid var size");
    JitCompiler_t comp;

    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end(); i++)
        PushStackValueJit (*i, &comp);
    comp.mov (comp.r_rax, int32_t($ dllResolved_[arg.arg1]));
    comp.call (comp.r_rax);
    if (! $ isVar (arg.arg2)) ($ GetReg (arg.arg2)).MovFromReg (&comp, comp.r_rax);
    else comp.mov ((DWORD*)$ GetPtr (arg.flag2, arg.arg2), comp.r_rax);
    comp.add(comp.r_rsp, $ RspAdd());
    comp.retn();
    //comp.mov(ptr, comp.r_rax);
    comp.BuildAndRun();
FUNCTION_END

FUNCTION_BEGIN(JIT_Call_QWord, 1, 3, ARG_DLL_FUNC _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER)
    long long res = 0;
    if ($ GetSize(arg.flag2, arg.arg2) != sizeof (QWORD))
        return ErrorReturn_t (RET_ERROR_CONTINUE, $ isReg (arg.flag2) ? "Invalid reg size" : "Invalid var size");
    JitCompiler_t comp;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end(); i++)
        PushStackValueJit (*i, &comp);
    comp.mov (comp.r_rax, int32_t($ dllResolved_[arg.arg1]));
    comp.call (comp.r_rax);
    comp.mov((DWORD*)&res, comp.r_rax);
    comp.mov((DWORD*)&res + 1, comp.r_rdx);
    comp.add(comp.r_rsp, $ RspAdd());
    comp.retn();
    comp.BuildAndRun();
    if (! $ isVar (arg.arg2)) ($ GetReg (arg.arg2)).Set (res);
    else *(QWORD*)$ GetPtr (arg.flag2, arg.arg2) = res;
FUNCTION_END

#undef ASM_FILL_STACK
#undef ASM_CALL_FUNC

#define ARITHMETIC_FUNCTION(name, operator, check0) \
FUNCTION_BEGIN(name, 2, 3, ARG_VAR _ ARG_REG _ ARG_VAR _ ARG_REG _ ARG_NUM) \
long long opVal = $ GetVal (arg.flag2, arg.arg2); \
if (check0 && opVal == 0) \
{ \
return ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)"); \
} \
$ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag1, arg.arg1) operator opVal); \
FUNCTION_END

FUNCTION_BEGIN(Add, 2, 3, ARG_VAR _ ARG_REG _ ARG_VAR _ ARG_REG _ ARG_NUM)
JitCompiler_t comp;
//comp.add (comp.r_rax, 1);
comp.push<long long> (comp.r_rax);
//long long var = 0;
        //ErrorPrintfBox ("ADDRESS %X", &var);
comp.mov (comp.r_rax, (long long*) $ GetPtr(arg.flag2, arg.arg2));
//comp.add (comp.r_rax, 5LL);
comp.mov ((long long*) $ GetPtr(arg.flag1, arg.arg1), comp.r_rax);
//comp.add (comp.r_rax, (long long*) $ GetPtr(arg.flag2, arg.arg2));
/*if ($ isReg (arg.flag1)) $ GetReg (arg.arg2) . MovFromReg (&comp, comp.r_rax);
else
{
    switch ($ GetSize (arg.flag2, arg.arg2))
    {
        #define SIZE_CASE(type) \
        case sizeof (type): \
            comp.mov ((type*) $ GetPtr(arg.flag1, arg.arg1), comp.r_rax); \
            break;

        SIZE_CASE(char)
        SIZE_CASE(short)
        SIZE_CASE(long)
        SIZE_CASE(long long)

        #undef SIZE_CASE
    }
}*/


comp.add (comp.r_rsp, 4);
comp.retn();
comp.BuildAndRun();

FUNCTION_END

//ARITHMETIC_FUNCTION (Add, +, false)
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
return ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)"); \
} \
$ dataStack_.push(StackData_t (val0 operator val1, sizeof (long long))); \
FUNCTION_END


STACK_ARITHMETIC_FUNCTION (Adds, +, false)
STACK_ARITHMETIC_FUNCTION (Subs, -, false)
STACK_ARITHMETIC_FUNCTION (Muls, *, false)
STACK_ARITHMETIC_FUNCTION (Divs, /, true)
#undef STACK_ARITHMETIC_FUNCTION


#undef $

#undef _
