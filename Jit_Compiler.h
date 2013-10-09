#ifndef JIT_COMPILER_H_INCLUDED
#define JIT_COMPILER_H_INCLUDED

#define R_EAX 0b000
#define R_ECX 0b001
#define R_EDX 0b010
#define R_EBX 0b011
#define R_ESP 0b100
#define SIB   0b100
#define R_EBP 0b101
#define OFF   0b101
#define R_ESI 0b110
#define R_EDI 0b111

enum RMModes
{
    MODE_ADDRESS       = 0b00,
    MODE_ADDRESS_BYTE  = 0b01,
    MODE_ADDRESS_DWORD = 0b10,
    MODE_REGISTER      = 0b11
};

#include "MCode.h"
#include "Instruction.h"

struct RegisterInfo_t
{
    int8_t reg;
    RegisterInfo_t (int8_t reg_) :
        reg (reg_)
    {}
};

#define CHECK_64(T) if (sizeof (T) == 8) man.Emit64Prefix();

class JitCompiler_t
{
    InstructionManager_t man;

    public:

    RegisterInfo_t r_eax,
                   r_ecx,
                   r_edx,
                   r_ebx,
                   r_esp,
                   sib,
                   r_ebp,
                   off,
                   r_esi,
                   r_edi;

    JitCompiler_t () :
        man   (),
        r_eax (R_EAX),
        r_ecx (R_ECX),
        r_edx (R_EDX),
        r_ebx (R_EBX),
        r_esp (R_ESP),
        sib   (SIB),
        r_ebp (R_EBP),
        off   (OFF),
        r_esi (R_ESI),
        r_edi (R_EDI)
    {}

    void mov (RegisterInfo_t regDest, RegisterInfo_t regSrc)
    {
        CHECK_64(T)
        man.EmitMov (regDest.reg, regSrc.reg);
    }

    template <typename T>
    void mov (RegisterInfo_t regDest, T imm)
    {
        man.EmitMov (regDest.reg, imm);
    }

    template <typename T>
    void mov (T* pointer, RegisterInfo_t regSrc)
    {
        man.EmitMov (pointer, regSrc.reg);
    }

    template <typename T>
    void push (T imm)
    {
        man.EmitPush (imm);
    }

    void call (RegisterInfo_t regDest)
    {
        man.EmitCall (regDest.reg);
    }

    template <typename T>
    void add (RegisterInfo_t regDest, T imm)
    {
        man.EmitAdd (regDest.reg, imm);
    }

    template <typename T>
    void add (RegisterInfo_t regDest, T* src)
    {
        man.EmitAdd (regDest.reg, src);
    }

    template <typename T>
    void add (T* dest, RegisterInfo_t regSrc)
    {
        man.EmitAdd (dest, regSrc.reg);
    }

    template <typename T>
    void sub (RegisterInfo_t regDest, T imm)
    {
        man.EmitSub (regDest.reg, imm);
    }

    template <typename T>
    void sub (RegisterInfo_t regDest, T* src)
    {
        man.EmitSub (regDest.reg, src);
    }

    template <typename T>
    void sub (T* dest, RegisterInfo_t regSrc)
    {
        man.EmitSub (dest, regSrc.reg);
    }

    template <typename T>
    void mul (RegisterInfo_t regDest, RegisterInfo_t regSrc, T imm)
    {
        man.EmitMul (regDest.reg, regSrc.reg, imm);
    }

    template <typename T>
    void mul (RegisterInfo_t regDest, T* src)
    {
        man.EmitMul (regDest.reg, src);
    }

    void mul (RegisterInfo_t regDest, RegisterInfo_t regSrc)
    {
        man.EmitMul (regDest.reg, regSrc.reg);
    }

    void div (RegisterInfo_t regSrc)
    {
        man.EmitDiv (regSrc.reg);
    }

    template <typename T>
    void div (T* src)
    {
        man.EmitDiv (src);
    }

    void retn ()
    {
        man.EmitRetn ();
    }

    void BuildAndRun ()
    {
        man.BuildAndRun();
    }

};

#undef R_EAX
#undef R_ECX
#undef R_EDX
#undef R_EBX
#undef R_ESP
#undef SIB
#undef R_EBP
#undef OFF
#undef R_ESI
#undef R_EDI

#endif // JIT_COMPILER_H_INCLUDED
