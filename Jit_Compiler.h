#ifndef JIT_COMPILER_H_INCLUDED
#define JIT_COMPILER_H_INCLUDED

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

public:
    JitCompiler_t () :
        buffer_ ()
    {}

    void PushDword (DWORD dw)
    {
        buffer_.push_back (0x68);
        PushNum (dw);
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
    }
    void CallEax ()
    {
        buffer_.push_back (0xFF);
        buffer_.push_back (0xD0);
    }
    void AddToEsp (DWORD dw)
    {
        buffer_.push_back (0x81);
        buffer_.push_back (0xC4);
        PushNum (dw);
    }
    void MovEaxToPtr (DWORD dw)
    //! mov [dw], eax
    {
        buffer_.push_back (0xA3);
        PushNum (dw);
    }
    void MovEdxToPtr (DWORD dw)
    //! mov [dw], edx
    {
        buffer_.push_back (0x89);
        buffer_.push_back (0x15);
        PushNum (dw);
    }
    void Retn ()
    {
        buffer_.push_back (0xCC);
        buffer_.push_back (0xC3);
    }

    void Run ()
    {
        fclose (f);
        unsigned char* func = new unsigned char [buffer_.size() + 1];
        memcpy (func, buffer_.data(), buffer_.size());
        ((void (*) ())func) ();
        delete [] func;
        func = nullptr;
    }
};

#endif // JIT_COMPILER_H_INCLUDED
