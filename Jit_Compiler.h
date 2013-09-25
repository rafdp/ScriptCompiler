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

class JitCompiler_t
{
    std::vector<char> buffer_;
    template <typename T>
    void PushNum (T num)
    {
        for (int i = 0; i < sizeof (T); i++)
        {
            buffer_.push_back (char (num >> i * 8));
        }
    }

    InstructionManager_t man;
public:
    JitCompiler_t () :
        buffer_ (),
        man     ()
    {}

    void PushDword (DWORD dw)
    {
        buffer_.push_back (0x68);
        PushNum (dw);
        man.EmitPush(dw);
    }
    void PushWord (WORD w)
    {
        buffer_.push_back (0x66);
        buffer_.push_back (0x68);
        PushNum (w);
    }
    void MovToEax (DWORD dw)
    {
        buffer_.push_back (0xB8);
        PushNum (dw);
        man.EmitMov(R_EAX, dw);
    }
    void CallEax ()
    {
        buffer_.push_back (0xFF);
        buffer_.push_back (0xD0);
        man.EmitCall(R_EAX);
    }
    void AddToEsp (DWORD dw)
    {
        buffer_.push_back (0x81);
        buffer_.push_back (0xC4);
        PushNum (dw);
        man.EmitAdd(R_ESP, dw);
    }
    void MovEaxToPtr (DWORD dw)
    //! mov [dw], eax
    {
        buffer_.push_back (0xA3);
        PushNum (dw);
        man.EmitMov((void*)dw, R_EAX);
    }
    void MovEdxToPtr (DWORD dw)
    //! mov [dw], edx
    {
        buffer_.push_back (0x89);
        buffer_.push_back (0x15);
        PushNum (dw);
        man.EmitMov((void*)dw, R_EDX);
    }
    void Retn ()
    {
        //buffer_.push_back (0xCC);
        buffer_.push_back (0xC3);
        man.EmitRetn();
    }

    void Run ()
    {
        man.SaveData();
        FILE* f = fopen ("Jit/dump.bin", "wb");
        fwrite (buffer_.data(), 1, buffer_.size(), f);
        fclose (f);
        /*unsigned char* func = new unsigned char [buffer_.size() + 1];
        memcpy (func, buffer_.data(), buffer_.size());
        //VirtualProtect(func, buffer_.size(), PAGE_EXECUTE_READWRITE, nullptr);
        ((void (*) ())func) ();
        delete [] func;
        func = nullptr;*/


        unsigned char* func = new unsigned char [man.emitter_.mcode_.buffer_.size() + 1];
        memcpy (func, man.emitter_.mcode_.buffer_.data(), man.emitter_.mcode_.buffer_.size());
        //VirtualProtect(func, buffer_.size(), PAGE_EXECUTE_READWRITE, nullptr);
        ((void (*) ())func) ();
        delete [] func;
        func = nullptr;
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
