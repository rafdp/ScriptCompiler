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
#define CHECK_64(T) //if (sizeof (T) == 8) {ErrorPrintfBox ("%d %s", __LINE__, __PRETTY_FUNCTION__); man.Emit64Prefix ();}

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
        man.EmitMov (regDest, regSrc);
    }

    template <typename T>
    void mov (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64 (T)
        man.EmitMov (regDest, (int32_t)imm);
    }

    template <typename T1, typename T2>
    void mov (T1* ptr, T2 imm)
    {
        CHECK_64 (T1)
        man.EmitMov ((int32_t*)ptr, (int32_t)imm);
    }

    template <typename T>
    void mov (T* pointer, CPURegisterInfo_t regSrc)
    {
        CHECK_64 (T)
        man.EmitMov (pointer, regSrc);
    }

    template <typename T>
    void mov (CPURegisterInfo_t regDest, T* pointer)
    {
        CHECK_64 (T)
        man.EmitMov (regDest, pointer);
    }

    template <typename T = long>
    void push (CPURegisterInfo_t reg)
    {
        CHECK_64 (T)
        man.EmitPush (reg);
    }

    template <typename T = long>
    void pop (CPURegisterInfo_t reg)
    {
        CHECK_64 (T)
        man.EmitPop (reg);
    }

    template <typename T>
    void push (T imm)
    {
        if (sizeof (imm) == sizeof (int64_t))
        {
            //ErrorPrintfBox ("%d %s", __LINE__, __PRETTY_FUNCTION__);
            man.EmitPush (DWORD ((int64_t)imm >> (sizeof (int32_t)*8)));
            man.EmitPush (DWORD ((int64_t)imm));
        }
        else man.EmitPush (imm);
    }

    void call (CPURegisterInfo_t regDest)
    {
        man.EmitCall (regDest);
    }

    template <typename T>
    void call (T* ptr)
    {
        man.EmitCall (ptr);
    }

    void callr (int32_t rel)
    {
        man.EmitCallr (rel);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64 (T)
        man.EmitAdd (regDest, src);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64 (T)
        man.EmitAdd (regDest, imm);
    }

    template <typename T>
    void add (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_64 (T)
        man.EmitAdd (dest, regSrc);
    }

    template <typename T>
    void adc (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64 (T)
        man.EmitAdc (regDest, src);
    }

    template <typename T>
    void adc (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64 (T)
        man.EmitAdc (regDest, imm);
    }

    template <typename T>
    void adc (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_64 (T)
        man.EmitAdc (dest, regSrc);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64 (T)
        man.EmitSub (regDest, imm);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64 (T)
        man.EmitSub (regDest, src);
    }

    template <typename T>
    void sub (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_64 (T)
        man.EmitSub (dest, regSrc);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_64 (T)
        man.EmitSbb (regDest, imm);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64 (T)
        man.EmitSbb (regDest, src);
    }

    template <typename T>
    void sbb (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_64 (T)
        man.EmitSbb (dest, regSrc);
    }

    template <typename T>
    void mul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc, T imm)
    {
        CHECK_64 (T)
        man.EmitMul (regDest, regSrc, imm);
    }

    template <typename T>
    void mul (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_64 (T)
        man.EmitMul (regDest, src);
    }

    void mul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitMul (regDest, regSrc);
    }

    void div (CPURegisterInfo_t regSrc)
    {
        man.EmitDiv (regSrc);
    }

    template <typename T>
    void div (T* src)
    {
        CHECK_64 (T)
        man.EmitDiv (src);
    }

    void retn ()
    {
        man.EmitRetn ();
    }

    void BuildAndRun ()
    {
        man.BuildAndRun ();
    }

    void int3 ()
    {
        man.EmitInt3 ();
    }

    template <typename T>
    void inc (T* ptr)
    {
        man.EmitInc (ptr);
    }

    void inc (CPURegisterInfo_t regSrc)
    {
        man.EmitInc (regSrc);
    }

    template <typename T>
    void cmp (T* pointer, CPURegisterInfo_t regSrc)
    {
        man.EmitCmp (pointer, regSrc);
    }

    template <typename T>
    void cmp (CPURegisterInfo_t regDest, T* pointer)
    {
        man.EmitCmp (regDest, pointer);
    }

    template <typename T1, typename T2>
    void cmp (T1* ptr, T2 imm)
    {
        CHECK_64 (T1)
        man.EmitCmp (ptr, (int32_t)imm);
    }

    void jmp (int32_t rel)
    {
        man.EmitJmp (rel);
    }

    void jl (int32_t rel)
    {
        man.EmitJl (rel);
    }

    void jge (int32_t rel)
    {
        man.EmitJge (rel);
    }

    void je (int32_t rel)
    {
        man.EmitJe (rel);
    }

    void jne (int32_t rel)
    {
        man.EmitJne (rel);
    }

    void jle (int32_t rel)
    {
        man.EmitJle (rel);
    }

    void jg (int32_t rel)
    {
        man.EmitJg (rel);
    }

    size_t Size ()
    {
        return man.Size ();
    }

    std::vector<uint8_t>* GetData ()
    {
        return man.GetData ();
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
