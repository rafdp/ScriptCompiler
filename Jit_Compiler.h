#ifndef JIT_COMPILER_H_INCLUDED
#define JIT_COMPILER_H_INCLUDED

#define R_RAX 0b000
#define R_RCX 0b001
#define R_RDX 0b010
#define R_RBX 0b011
#define R_RSP 0b100
#define SIB   0b100
#define R_RBP 0b101
#define OFF   0b101
#define R_RSI 0b110
#define R_RDI 0b111

enum RMModes
{
    MODE_ADDRESS       = 0b00,
    MODE_ADDRESS_BYTE  = 0b01,
    MODE_ADDRESS_DWORD = 0b10,
    MODE_REGISTER      = 0b11
};

#include "MCode.h"
#include "Instruction.h"

struct CPURegisterInfo_t
{
    int8_t reg;
    CPURegisterInfo_t (int8_t reg_) :
        reg (reg_)
    {}
};

#define CHECK_64(T) if (sizeof (T) == 8) {ErrorPrintfBox ("%d %s", __LINE__, __PRETTY_FUNCTION__); man.Emit64Prefix();}

class JitCompiler_t
{
    InstructionManager_t man;

    public:

    CPURegisterInfo_t r_rax,
                   r_rcx,
                   r_rdx,
                   r_rbx,
                   r_rsp,
                   sib,
                   r_rbp,
                   off,
                   r_rsi,
                   r_rdi;

    JitCompiler_t () :
        man   (),
        r_rax (R_RAX),
        r_rcx (R_RCX),
        r_rdx (R_RDX),
        r_rbx (R_RBX),
        r_rsp (R_RSP),
        sib   (SIB),
        r_rbp (R_RBP),
        off   (OFF),
        r_rsi (R_RSI),
        r_rdi (R_RDI)
    {}

    void mov (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitMov (regDest.reg, regSrc.reg);
    }

    template <typename T>
    void mov (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64(T)
        man.EmitMov (regDest.reg, imm);
    }

    template <typename T>
    void mov (T* pointer, CPURegisterInfo_t regSrc)
    {
        CHECK_64(T)
        man.EmitMov (pointer, regSrc.reg);
    }

    template <typename T>
    void mov (CPURegisterInfo_t regDest, T* pointer)
    {
        CHECK_64(T)
        man.EmitMov (regDest.reg, pointer);
    }

    template <typename T>
    void push (CPURegisterInfo_t reg)
    {
        CHECK_64(T)
        man.EmitPush (reg.reg);
    }

    template <typename T>
    void push (T imm)
    {
        if (sizeof (imm) == sizeof (int64_t))
        {
            //ErrorPrintfBox ("%d %s", __LINE__, __PRETTY_FUNCTION__);
            man.EmitPush (DWORD(imm >> (sizeof (int32_t)*8)));
            man.EmitPush (DWORD(imm));
        }
        else man.EmitPush (imm);
    }

    void call (CPURegisterInfo_t regDest)
    {
        man.EmitCall (regDest.reg);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64(T)
        man.EmitAdd (regDest.reg, src);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64(T)
        man.EmitAdd (regDest.reg, imm);
    }

    template <typename T>
    void add (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_64(T)
        man.EmitAdd (dest, regSrc.reg);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64(T)
        man.EmitSub (regDest.reg, imm);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64(T)
        man.EmitSub (regDest.reg, src);
    }

    template <typename T>
    void sub (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_64(T)
        man.EmitSub (dest, regSrc.reg);
    }

    template <typename T>
    void mul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc, T imm)
    {
        CHECK_64(T)
        man.EmitMul (regDest.reg, regSrc.reg, imm);
    }

    template <typename T>
    void mul (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64(T)
        man.EmitMul (regDest.reg, src);
    }

    void mul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitMul (regDest.reg, regSrc.reg);
    }

    void div (CPURegisterInfo_t regSrc)
    {
        man.EmitDiv (regSrc.reg);
    }

    template <typename T>
    void div (T* src)
    {
        CHECK_64(T)
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
