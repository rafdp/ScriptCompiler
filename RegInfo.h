#ifndef REGINFO_H_INCLUDED
#define REGINFO_H_INCLUDED
struct RegInfo_t
{
    char* reg;
    char size;
    RegInfo_t (char* registers, long long param, exception_data* expn) :
        reg (registers + param),
        size ()
    {
        switch (param)
        {
            case REG_ABX:
            case REG_BBX:
            case REG_CBX:
            case REG_DBX:
            case REG_RB:
                size = 1;
                break;
            case REG_AWX:
            case REG_BWX:
            case REG_CWX:
            case REG_DWX:
            case REG_RW:
                size = 2;
                break;
            case REG_ADX:
            case REG_BDX:
            case REG_CDX:
            case REG_DDX:
            case REG_RD:
                size = 4;
                break;
            case REG_AQX:
            case REG_BQX:
            case REG_CQX:
            case REG_DQX:
            case REG_RQ:
            case REG_PTBYTE:
            case REG_PTWORD:
            case REG_PTDWORD:
            case REG_PTQWORD:
                size = 8;
                break;
            default:
            NAT_EXCEPTION (expn, "Invalid register info", ERROR_INVALID_OP_SIZE_SWITCH)
        }
    }

/*
    #define TYPED_PTR_REGISTER (register) \
    register.GetPtr < (( (register).size == sizeof (long long)) ? (long long*) : \
    (( (register).size == sizeof (long)) ? (long*) : \
    (( (register).size == sizeof (short)) ? (short*) : \
    (( (register).size == sizeof (char)) ? (char*) : (void*)))))> ()
*/

    long long Get8byte ()
    {
        switch (size)
        {
            case 1:
                return static_cast<int64_t>(*reinterpret_cast<int8_t*> (reg));
            case 2:
                return static_cast<int64_t>(*reinterpret_cast<int16_t*> (reg));
            case 4:
                return static_cast<int64_t>(*reinterpret_cast<int32_t*> (reg));
            case 8:
                return static_cast<int64_t>(*reinterpret_cast<int64_t*> (reg));
            default:
                return 0;
        }
    }

    template <class T>
    void Set (T data)
    {
        switch (size)
        {
            #define SIZE_CASE(n) \
            case sizeof (int##n##_t): \
                memcpy (reg, &data, sizeof (int##n##_t)); \
                break;
            SIZE_CASE (8)
            SIZE_CASE (16)
            SIZE_CASE (32)
            SIZE_CASE (64)
            #undef SIZE_CASE
            default:
            break;
        }

        return;
    }

    void MovFromReg (JitCompiler_t* comp, CPURegisterInfo_t register_)
    {
        switch (size)
        {
            case 1:
                comp->mov (reinterpret_cast<int8_t*>  (reg), register_);
                break;
            case 2:
                comp->mov (reinterpret_cast<int16_t*> (reg), register_);
                break;
            case 4:
                comp->mov (reinterpret_cast<int32_t*> (reg), register_);
                break;
            case 8:
                comp->mov (reinterpret_cast<int64_t*> (reg), register_);
                break;
            default:
                return;
        }
    }

    void Mov64FromReg (JitCompiler_t* comp,
                       CPURegisterInfo_t register1_,
                       CPURegisterInfo_t register2_)
    {
        if (size == sizeof (long long))
        {
            comp->mov (reinterpret_cast<int32_t*> (reg), register1_);
            comp->mov (reinterpret_cast<int32_t*> (reg) + 1, register2_);
        }
    }
};


#endif
