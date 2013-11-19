#define _ ,

#define $ instance_->

#define FUNC_OPEN  (
#define FUNC_CLOSE )

#define FUNCTION_BEGIN(name, n_params1, n_params2, params) \
void VirtualProcessor_t::name () \
{ \
Arg_t& arg = $ args_[instance_->run_line_]; \
ManageInputArgs_t man (ExpectedArg_t (n_params1, n_params2, params), arg); \

#define FUNCTION_END \
currentReturn_ = RET_NO_ERRORS; \
}


FUNCTION_BEGIN (RebuildVar, 2, 4, ARG_VAR _ ARG_NULL _ ARG_NUM)
    VarData_t& var = $ vars_[arg.arg1];
    var.Delete ();
    var.code = $ isNum (arg.arg2) ? arg.arg2 : TYPE_QWORD;
    var.size = $ typeSizes_[var.code];
    var.Free ();
FUNCTION_END

FUNCTION_BEGIN (Push, 5, 0, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
    $ dataStack_.push (StackData_t ($ GetVal (arg.flag1, arg.arg1), $ GetSize (arg.flag1, arg.arg1)));
FUNCTION_END

FUNCTION_BEGIN (Pop, 4, 0, ARG_NULL _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
    long long val = $ dataStack_.pop ().data;
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
        /*ErrorPrintfBox ("MOV before\n%s %lld %lld\n%s %lld %lld\n",
                        ARG_D[arg.flag1].c_str (), arg.arg1, $ GetVal (arg.flag1, arg.arg1),
                        ARG_D[arg.flag2].c_str (), arg.arg2, $ GetVal (arg.flag2, arg.arg2));
        long long res = $ GetVal (arg.flag2, arg.arg2);*/
        $ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag2, arg.arg2));

        /*ErrorPrintfBox ("MOV after\n%s %lld %lld\n%s %lld %lld\n",
                        ARG_D[arg.flag1].c_str (), arg.arg1, $ GetVal (arg.flag1, arg.arg1),
                        ARG_D[arg.flag2].c_str (), arg.arg2, $ GetVal (arg.flag2, arg.arg2));
        */
    }

FUNCTION_END

FUNCTION_BEGIN (Lea, 2, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_STR)
    if ($ GetVarType (arg.flag1, arg.arg1) != TYPE_PTR)
        {currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, "arg1 is not a pointer"); return;}
    if ($ isVar (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, (long long) ($ GetVarPt (arg.flag2, arg.arg2)));
    else
    if ($ isReg (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, (long long) ($ GetReg (arg.arg2).reg));
    else
    if ($ isStr (arg.flag2))
        $ SetVal (arg.flag1, arg.arg1, (long long) ($ GetVal (arg.flag2, arg.arg2)));
FUNCTION_END

FUNCTION_BEGIN (Print, 1, 5, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_STR)
    //ErrorPrintfBox ("Called Print\n");
    const char error0[] = "Null pointer, cannot print str";
    const char error1[] = "Invalid cmd";

    #define _RET_ERROR_(n) {currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, error##n); return;}

    #define PRINT_FORMATTED(val) \
    {\
    switch (arg.arg1) \
        { \
            case PRINT_STRING: \
                if (!val) _RET_ERROR_ (0) \
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
        PRINT_FORMATTED ( ($ strings_[arg.arg2]))
    else
    if ($ isNum (arg.flag2) && arg.arg2 == PRINT_STACK)
        PRINT_FORMATTED ($ dataStack_.top ())
    else _RET_ERROR_ (1)

    #undef _RET_ERROR_
FUNCTION_END

FUNCTION_BEGIN (Cmpr, 4, 4, ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)

    long long val1 = $ GetVal (arg.flag1, arg.arg1),
              val2 = $ GetVal (arg.flag2, arg.arg2);

    ErrorPrintfBox ("CMPR val1 = %x, val2 = %x line %d", $ GetPtr (arg.flag1, arg.arg1), $ GetPtr (arg.flag2, arg.arg2), $ run_line_);

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
   // ErrorPrintfBox ("Returning to line %d", $ callStack_.top ().retLine);
    $ run_line_ = $ callStack_.top ().retLine;
    $ callStack_.pop ();
    //$ callStack_.Dump ();
    //ErrorPrintfBox ("Returned to line %d", $ run_line_);
FUNCTION_END

FUNCTION_BEGIN (Call, 2, 0, ARG_FUNC _ ARG_FUNC_MEMBER)
    if (arg.flag1 == ARG_FUNC_MEMBER)
        $ ComplexCall (arg.arg1);
    else
    {
        $ callStack_.push (CallInfo_t ($ run_line_));
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
    std::string code;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++)
        code += "push " + GetAsmNumString (*i) + "\n";
    code += "mov eax, " + GetAsmNumString (int (printf)) + "\ncall eax\n";
    code += "add esp, " + GetAsmNumString ( ($ dataStack_.size ()) * 4, "") + "\nretn\n";
    //RunAsm (code);
FUNCTION_END

#define ASM_FILL_STACK(str) \
for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++) \
PushStackValueString (*i, &str);/*str += "push " + GetAsmNumString (*i) + "\n"; */

#define ASM_CALL_FUNC(ptr, str) \
str += "mov eax, " + GetAsmNumString (int (ptr)) + "\ncall eax\n";

FUNCTION_BEGIN (JIT_Call_Void, 1, 0, ARG_DLL_FUNC)
    //ErrorPrintfBox ("%d %d %s\n", $ stackDumpPoint_, $ dataStack_.size (), __PRETTY_FUNCTION__);

    JitCompiler_t comp;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++)
        PushStackValueJit (*i, &comp);
    comp.mov (comp.r_rax, int32_t ($ dllResolved_[arg.arg1]));
    comp.call (comp.r_rax);
    comp.add (comp.r_rsp, (long) $ RspAdd ());
    comp.retn ();
    comp.BuildAndRun ();

FUNCTION_END

FUNCTION_BEGIN (JIT_Call_DWord, 1, 3, ARG_DLL_FUNC _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER)
    if ($ GetSize (arg.flag2, arg.arg2) != sizeof (DWORD))
        currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, $ isReg (arg.flag2) ? "Invalid reg size" : "Invalid var size");
    JitCompiler_t comp;

    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++)
        PushStackValueJit (*i, &comp);
    comp.mov (comp.r_rax, int32_t ($ dllResolved_[arg.arg1]));
    comp.call (comp.r_rax);
    if (! $ isVar (arg.arg2)) ($ GetReg (arg.arg2)).MovFromReg (&comp, comp.r_rax);
    else comp.mov ((DWORD*)$ GetPtr (arg.flag2, arg.arg2), comp.r_rax);
    comp.add (comp.r_rsp, $ RspAdd ());
    comp.retn ();
    //comp.mov (ptr, comp.r_rax);
    comp.BuildAndRun ();
FUNCTION_END

FUNCTION_BEGIN (JIT_Call_QWord, 1, 3, ARG_DLL_FUNC _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER)
    long long res = 0;
    if ($ GetSize (arg.flag2, arg.arg2) != sizeof (QWORD))
        currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, $ isReg (arg.flag2) ? "Invalid reg size" : "Invalid var size");
    JitCompiler_t comp;
    for (stack<StackData_t>::iterator i (& $ dataStack_, & $ dataStack_[$ stackDumpPoint_]); i < $ dataStack_.end (); i++)
        PushStackValueJit (*i, &comp);
    comp.mov (comp.r_rax, int32_t ($ dllResolved_[arg.arg1]));
    comp.call (comp.r_rax);
    comp.mov ((DWORD*)&res, comp.r_rax);
    comp.mov ((DWORD*)&res + 1, comp.r_rdx);
    comp.add (comp.r_rsp, $ RspAdd ());
    comp.retn ();
    comp.BuildAndRun ();
    if (! $ isVar (arg.arg2)) ($ GetReg (arg.arg2)).Set (res);
    else *(QWORD*)$ GetPtr (arg.flag2, arg.arg2) = res;
FUNCTION_END

#undef ASM_FILL_STACK
#undef ASM_CALL_FUNC

#define ARITHMETIC_FUNCTION(name, operator, check0) \
FUNCTION_BEGIN (name, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM) \
long long opVal = $ GetVal (arg.flag2, arg.arg2); \
if (check0 && opVal == 0) \
{ \
currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)"); \
} \
$ SetVal (arg.flag1, arg.arg1, $ GetVal (arg.flag1, arg.arg1) operator opVal); \
FUNCTION_END

#define SIZE_CASE(type) \
case sizeof (type): \
    comp.mov ((type*) $ GetPtr (arg.flag1, arg.arg1), comp.r_rax); \
    break;

#define ARG_SIZE_SWITCH_PTR_LONG(num, offset, preEmission, postEmission) \
    switch ($ GetSize (arg.flag##num, arg.arg##num)) \
    { \
        case sizeof (char): \
            preEmission (char*) $ GetPtr (arg.flag##num, arg.arg##num) + offset postEmission; \
            break; \
        case sizeof (short): \
            preEmission (short*) $ GetPtr (arg.flag##num, arg.arg##num) + offset postEmission; \
            break; \
        case sizeof (long): \
        case sizeof (long long): \
            preEmission (long*) $ GetPtr (arg.flag##num, arg.arg##num) + offset postEmission; \
            break; \
    }

#define ARITHMETIC_FUNCTION_ADD_SUB_MUL_BASE(func) \
{ \
    JitCompiler_t comp; \
    comp.push (comp.r_rax); \
    ARG_SIZE_SWITCH_PTR_LONG (1, 0, comp.mov FUNC_OPEN comp.r_rax _ , FUNC_CLOSE) \
    ARG_SIZE_SWITCH_PTR_LONG (2, 0, comp.func FUNC_OPEN comp.r_rax _ , FUNC_CLOSE) \
    if ($ isReg (arg.flag1)) $ GetReg (arg.arg1) . MovFromReg (&comp, comp.r_rax); \
    else \
    { \
        ARG_SIZE_SWITCH_PTR_LONG (1, 0, comp.mov FUNC_OPEN , _ comp.r_rax FUNC_CLOSE) \
    } \
    comp.pop (comp.r_rax); \
    comp.retn (); \
    comp.BuildAndRun (); \
}

#define ARITHMETIC_FUNCTION_ADD_SUB_64_PART(func1, func2) \
bool first64 = $ GetSize (arg.flag1, arg.arg1) > sizeof (DWORD); \
bool second64 = $ GetSize (arg.flag2, arg.arg2) > sizeof (DWORD); \
if (first64 || \
    second64) \
{ \
    JitCompiler_t comp; \
    comp.push (comp.r_rax); \
    comp.push (comp.r_rdx); \
 \
    ARG_SIZE_SWITCH_PTR_LONG (1, 0, comp.mov FUNC_OPEN comp.r_rax _ , FUNC_CLOSE) \
    if (first64) \
        ARG_SIZE_SWITCH_PTR_LONG (1, 1, comp.mov FUNC_OPEN comp.r_rdx _ , FUNC_CLOSE) \
 \
    ARG_SIZE_SWITCH_PTR_LONG (2, 0, comp.func1 FUNC_OPEN comp.r_rax _ , FUNC_CLOSE) \
    if (second64 && first64) \
        ARG_SIZE_SWITCH_PTR_LONG (2, 1, comp.func2 FUNC_OPEN comp.r_rdx _ , FUNC_CLOSE) \
 \
    if ($ isReg (arg.flag1)) \
    { \
        if (first64) \
            $ GetReg (arg.arg1) . Mov64FromReg (&comp, comp.r_rax, comp.r_rdx); \
        else \
            $ GetReg (arg.arg1) . MovFromReg (&comp, comp.r_rax); \
    } \
    else \
    { \
        ARG_SIZE_SWITCH_PTR_LONG (1, 0, comp.mov FUNC_OPEN , _ comp.r_rax FUNC_CLOSE) \
        if (first64) \
            ARG_SIZE_SWITCH_PTR_LONG (1, 1, comp.mov FUNC_OPEN , _ comp.r_rdx FUNC_CLOSE) \
 \
    } \
 \
    comp.pop (comp.r_rdx); \
    comp.pop (comp.r_rax); \
    comp.retn (); \
    comp.BuildAndRun (); \
} \
else


FUNCTION_BEGIN (Add, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)

ARITHMETIC_FUNCTION_ADD_SUB_64_PART (add, adc)
ARITHMETIC_FUNCTION_ADD_SUB_MUL_BASE (add)

FUNCTION_END

FUNCTION_BEGIN (Sub, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)

ARITHMETIC_FUNCTION_ADD_SUB_64_PART (sub, sbb)
ARITHMETIC_FUNCTION_ADD_SUB_MUL_BASE (sub)

FUNCTION_END

FUNCTION_BEGIN (Mul, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)
if ($ GetSize (arg.flag1, arg.arg1) > sizeof (DWORD) ||
    $ GetSize (arg.flag2, arg.arg2) > sizeof (DWORD))
    {
        //ErrorPrintfBox ("SIZE %d %d\n", $ GetSize (arg.flag1, arg.arg1), $ GetSize (arg.flag2, arg.arg2));
    $ SetVal (arg.flag1, arg.arg1,
              $ GetVal (arg.flag1, arg.arg1) * $ GetVal (arg.flag2, arg.arg2));
    }
else
ARITHMETIC_FUNCTION_ADD_SUB_MUL_BASE (mul)
FUNCTION_END


//ARITHMETIC_FUNCTION_ADD_SUB_MUL (Add, +, add)
//ARITHMETIC_FUNCTION_ADD_SUB_MUL (Sub, -, sub)
//ARITHMETIC_FUNCTION_ADD_SUB_MUL (Mul, *, mul)
/*
FUNCTION_BEGIN (Div, 3, 4, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG _ ARG_NUM)
long long opVal = $ GetVal (arg.flag2, arg.arg2);

if (opVal == 0)
{
    return ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)");
}

if ($ GetSize (arg.flag2, arg.arg2) > sizeof (DWORD))
    $ SetVal (arg.flag1, arg.arg1,
              $ GetVal (arg.flag1, arg.arg1) / opVal);
else
{
    JitCompiler_t comp;
    comp.push (comp.r_rax);
    comp.push (comp.r_rdx);

    comp.mov (comp.r_rax, (long*) $ GetPtr (arg.flag1, arg.arg1));
    if ($ GetSize (arg.flag1, arg.arg1) > sizeof (DWORD))
        comp.mov (comp.r_rdx, (long*) $ GetPtr (arg.flag1, arg.arg1) + 1);
    else
        comp.mov (comp.r_rdx, 0);
    comp.div ((long*) $ GetPtr (arg.flag2, arg.arg2));
    if ($ isReg (arg.flag1)) $ GetReg (arg.arg1) . MovFromReg (&comp, comp.r_rax);
    else
    {
        switch ($ GetSize (arg.flag1, arg.arg1))
        {
            SIZE_CASE (char)
            SIZE_CASE (short)
            case sizeof (long long):
            comp.mov ((long*) $ GetPtr (arg.flag1, arg.arg1) + 1, 0);
            SIZE_CASE (long)
        }
    }

    comp.pop (comp.r_rdx);
    comp.pop (comp.r_rax);
    comp.retn ();
    comp.BuildAndRun ();
}
FUNCTION_END
 */


ARITHMETIC_FUNCTION (Div, /, true)


FUNCTION_BEGIN (Sqrt, 3, 0, ARG_VAR _ ARG_VAR_MEMBER _ ARG_REG)
    long long val = $ GetVal (arg.flag1, arg.arg1);
    if (val < 0)
        currentReturn_ = ErrorReturn_t (RET_ERROR_CONTINUE, "Cannot calculate square root of a negative value");
    $ SetVal (arg.flag1, arg.arg1, (long long) (sqrt (val)));
FUNCTION_END


//ARITHMETIC_FUNCTION (Add, +, false)
//ARITHMETIC_FUNCTION (Sub, -, false)
//ARITHMETIC_FUNCTION (Mul, *, false)
//ARITHMETIC_FUNCTION (Div, /, true)
#undef ARITHMETIC_FUNCTION
#undef ARITHMETIC_FUNCTION_ADD_SUB
#undef SIZE_CASE

#define STACK_ARITHMETIC_FUNCTION(name, operator, check0) \
FUNCTION_BEGIN (name, 0, 0, ARG_NULL) \
long long val1 = $ dataStack_.pop (); \
long long val0 = $ dataStack_.pop (); \
if (check0 && val1 == 0) \
{ \
currentReturn_ = ErrorReturn_t (RET_ERROR_FATAL, "Invalid second operand value (0)"); \
} \
$ dataStack_.push (StackData_t (val0 operator val1, sizeof (long long))); \
FUNCTION_END


STACK_ARITHMETIC_FUNCTION (Adds, +, false)
STACK_ARITHMETIC_FUNCTION (Subs, -, false)
STACK_ARITHMETIC_FUNCTION (Muls, *, false)
STACK_ARITHMETIC_FUNCTION (Divs, /, true)


#undef STACK_ARITHMETIC_FUNCTION


#undef $

#undef _

#undef FUNC_OPEN
#undef FUNC_CLOSE


#undef ARG_SIZE_SWITCH_PTR_LONG
#undef ARITHMETIC_FUNCTION_ADD_SUB_64_PART
#undef ARITHMETIC_FUNCTION_ADD_SUB_MUL_BASE
