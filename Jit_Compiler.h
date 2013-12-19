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

class JitCompiler_t
{
    public:
    InstructionManager_t man;



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


    template <typename T = int>
    void mov (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitMov<T> (regDest, regSrc);
    }

    template <typename T = int>
    void mov (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        man.EmitMov<T> (regDest, regSrc);
    }

    template <typename T = int>
    void mov (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitMov<T> (regDest, regSrc);
    }

    template <typename T>
    void mov (CPURegisterInfo_t regDest, T imm)
    {
        man.EmitMov<T> (regDest, imm);
    }

    template <typename T>
    void mov (CPURegisterInfo_t* regDest, T imm)
    {
        man.EmitMov<T> (regDest, imm);
    }

    template <typename T>
    void mov (T* ptr, T imm)
    {
        man.EmitMov<T> (ptr, imm);
    }

    template <typename T>
    void mov (T* pointer, CPURegisterInfo_t regSrc)
    {
        man.EmitMov<T> (pointer, regSrc);
    }

    template <typename T>
    void mov (CPURegisterInfo_t regDest, T* pointer)
    {
        man.EmitMov<T> (regDest, pointer);
    }

    template <typename T = long>
    void push (CPURegisterInfo_t reg)
    {
        man.EmitPush<T> (reg);
    }

    template <typename T = long>
    void pop (CPURegisterInfo_t reg)
    {
        man.EmitPop<T> (reg);
    }

    template <typename T = int32_t>
    void popf ()
    {
        man.EmitPopf<T> ();
    }

    template <typename T>
    void push (T imm)
    {
        if (sizeof (T) == sizeof (int64_t))
        {
            //ErrorPrintfBox ("%d %s", __LINE__, __PRETTY_FUNCTION__);
            man.EmitPush<int32_t> (static_cast<int32_t>(static_cast<int64_t>(imm) >> (sizeof (int32_t)*8)));
            man.EmitPush<int32_t> (static_cast<int32_t>(static_cast<int64_t>(imm)));
        }
        else man.EmitPush<T> (imm);
    }

    template <typename T = int32_t>
    void pushf ()
    {
        man.EmitPushf<T> ();
    }

    void call (CPURegisterInfo_t regDest)
    {
        man.EmitCall (regDest);
    }

    template <typename T>
    void call (T* ptr)
    {
        man.EmitCall<T> (ptr);
    }

    void callr (int64_t rel)
    {
        man.EmitCallr (rel);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, T* src)
    {
        man.EmitAdd<T> (regDest, src);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        man.EmitAdd<T> (regDest, regSrc);
    }

    template <typename T>
    void add (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitAdd<T> (regDest, regSrc);
    }

    template <typename T>
    void add (CPURegisterInfo_t regDest, T imm)
    {
        man.EmitAdd<T> (regDest, imm);
    }

    template <typename T>
    void add (CPURegisterInfo_t* regDest, T imm)
    {
        man.EmitAdd<T> (regDest, imm);
    }

    template <typename T>
    void add (T* dest, CPURegisterInfo_t regSrc)
    {
        man.EmitAdd<T> (dest, regSrc);
    }

    template <typename T = int>
    void addf (CPURegisterInfo_t regDest, int8_t data)
    {
        man.EmitAddF (regDest, data);
    }

    template <typename T>
    void adc (CPURegisterInfo_t regDest, T* src)
    {
        man.EmitAdc<T> (regDest, src);
    }

    template <typename T>
    void adc (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        man.EmitAdc<T> (regDest, regSrc);
    }

    template <typename T>
    void adc (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitAdc<T> (regDest, regSrc);
    }

    template <typename T>
    void adc (CPURegisterInfo_t regDest, T imm)
    {
        man.EmitAdc<T> (regDest, imm);
    }

    template <typename T>
    void adc (CPURegisterInfo_t* regDest, T imm)
    {
        man.EmitAdc<T> (regDest, imm);
    }

    template <typename T>
    void adc (T* dest, CPURegisterInfo_t regSrc)
    {
        man.EmitAdc<T> (dest, regSrc);
    }

    template <typename T = int>
    void adcf (CPURegisterInfo_t regDest, int8_t data)
    {
        man.EmitAdcF (regDest, data);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, T imm)
    {
        man.EmitSub<T> (regDest, imm);
    }

    template <typename T>
    void sub (CPURegisterInfo_t* regDest, T imm)
    {
        man.EmitSub<T> (regDest, imm);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, T* src)
    {
        man.EmitSub<T> (regDest, src);
    }

    template <typename T>
    void sub (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        man.EmitSub<T> (regDest, regSrc);
    }

    template <typename T>
    void sub (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitSub<T> (regDest, regSrc);
    }

    template <typename T>
    void sub (T* dest, CPURegisterInfo_t regSrc)
    {
        man.EmitSub<T> (dest, regSrc);
    }

    template <typename T = int>
    void subf (CPURegisterInfo_t regDest, int8_t data)
    {
        man.EmitSubF (regDest, data);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t regDest, T imm)
    {
        man.EmitSbb<T> (regDest, imm);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t* regDest, T imm)
    {
        man.EmitSbb<T> (regDest, imm);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t regDest, T* src)
    {
        man.EmitSbb<T> (regDest, src);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        man.EmitSbb<T> (regDest, regSrc);
    }

    template <typename T>
    void sbb (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitSbb<T> (regDest, regSrc);
    }

    template <typename T>
    void sbb (T* dest, CPURegisterInfo_t regSrc)
    {
        man.EmitSbb<T> (dest, regSrc);
    }

    template <typename T = int>
    void sbbf (CPURegisterInfo_t regDest, int8_t data)
    {
        man.EmitSbbF (regDest, data);
    }

    template <typename T>
    void mul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc, T imm)
    {
        man.EmitMul<T> (regDest, regSrc, imm);
    }

    template <typename T>
    void mul (CPURegisterInfo_t regDest, T* src)
    {
        man.EmitMul<T> (regDest, src);
    }

    template <typename T = int>
    void mul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitMul<T> (regDest, regSrc);
    }

    template <typename T = int>
    void div (CPURegisterInfo_t regSrc)
    {
        man.EmitDiv<T> (regSrc);
    }

    template <typename T>
    void div (T* src)
    {
        man.EmitDiv<T> (src);
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
    void cmp (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitCmp<T> (regDest, regSrc);
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

    template <typename T>
    void cmp (T* ptr, T imm)
    {
        man.EmitCmp (ptr, imm);
    }

    template <typename T>
    void cmp (CPURegisterInfo_t* reg, T imm)
    {
        man.EmitCmp (reg, imm);
    }

    template <typename T>
    void cmp (CPURegisterInfo_t reg, T imm)
    {
        man.EmitCmp<T> (reg, imm);
    }

    template <typename T>
    void cmp (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        man.EmitCmp<T> (regDest, regSrc);
    }

    template <typename T>
    void cmp (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        man.EmitCmp<T> (regDest, regSrc);
    }

    void jmp (int64_t rel)
    {
        man.EmitJmp (rel);
    }

    void jl (int64_t rel)
    {
        man.EmitJl (rel);
    }

    void jge (int64_t rel)
    {
        man.EmitJge (rel);
    }

    void je (int64_t rel)
    {
        man.EmitJe (rel);
    }

    void jne (int64_t rel)
    {
        man.EmitJne (rel);
    }

    void jle (int64_t rel)
    {
        man.EmitJle (rel);
    }

    void jg (int64_t rel)
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
