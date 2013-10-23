#ifndef REGINFO_H_INCLUDED
#define REGINFO_H_INCLUDED
struct RegInfo_t
{
    char* reg;
    char size;
    RegInfo_t (char* registers, long long param) :
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
        }
    }

/*
    #define TYPED_PTR_REGISTER(register) \
    register.GetPtr <(((register).size == sizeof (long long)) ? (long long*) : \
    (((register).size == sizeof (long)) ? (long*) : \
    (((register).size == sizeof (short)) ? (short*) : \
    (((register).size == sizeof (char)) ? (char*) : (void*)))))> ()
*/

    long long Get8byte ()
    {
        switch (size)
        {
            case 1:
                return (long long)*(int8_t*) reg;
            case 2:
                return (long long)*(int16_t*)reg;
            case 4:
                return (long long)*(int32_t*)reg;
            case 8:
                return (long long)*(int64_t*)reg;
        }
        return 0;
    }

    template <class T>
    void Set (T data)
    {
        switch (size)
        {
            case 1:
                *(int8_t*)reg = *(int8_t*)(&data);
                break;
            case 2:
                *(int16_t*)reg = *(int16_t*)(&data);
                break;
            case 4:
                *(int32_t*)reg = *(int32_t*)(&data);
                break;
            case 8:
                *(int64_t*)reg = *(int64_t*)(&data);
                break;
        }

        return;
    }

    void MovFromReg (JitCompiler_t* comp, CPURegisterInfo_t register_)
    {
        switch (size)
        {
            case 1:
                comp->mov ((char*)reg,      register_);
                break;
            case 2:
                comp->mov ((short*)reg,     register_);
                break;
            case 4:
                comp->mov ((long*)reg,      register_);
                break;
            case 8:
                comp->mov ((long long*)reg, register_);
                break;
        }
    }
};


#endif
