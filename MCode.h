#ifndef MCODE_H_INCLUDED
#define MCODE_H_INCLUDED

class MCode_t
{
    public:
    std::vector <uint8_t> buffer_;

    public:
    MCode_t () :
        buffer_ ()
    {}

    template <typename Strct_Type, typename Ret_Type>
    void EmitData (Ret_Type (Strct_Type::*val))
    {
        for (int i = 0; i < sizeof (Ret_Type (Strct_Type::*)); i++)
        {
            buffer_.push_back (reinterpret_cast<uint8_t> (reinterpret_cast<int32_t>(reinterpret_cast<void*>(val)) >> i * 8));
        }
    }

    template <typename T>
    void EmitData (T* val)
    {
        for (int i = 0; i < sizeof (void*); i++)
        {
            buffer_.push_back (static_cast<uint8_t>(reinterpret_cast<int64_t>(val) >> i * 8));
        }
    }

    template <typename T>
    void EmitData (T val)
    {
        for (uint32_t i = 0; i < sizeof (T); i++)
        {
            buffer_.push_back (uint8_t (val >> i * 8));
        }
    }

    template<typename T>
    void AddToTop (T val)
    {
        buffer_.back() += static_cast <uint8_t> (val);
    }

    void BuildAndRun ()
    {
        //uint8_t* func = new uint8_t [buffer_.size () + 1];
        uint8_t* func = reinterpret_cast<uint8_t*> (VirtualAlloc (nullptr,
                                                                  buffer_.size () + 1,
                                                                  MEM_COMMIT | MEM_RESERVE,
                                                                  PAGE_EXECUTE_READWRITE));
        memcpy (func, buffer_.data (), buffer_.size ());
        //VirtualProtect (func, buffer_.size (), PAGE_EXECUTE_READWRITE, nullptr);
        //for (size_t i = 0; i < buffer_.size (); i++) printf ("%02X ", buffer_[i]);
        //printf ("\n");
        //ErrorPrintfBox ("A %x %x\n", this, &buffer_);
        //DebugBreak ();
        (reinterpret_cast<void (*)()> (func)) ();
        //ErrorPrintfBox ("B %x %x\n", this, &buffer_);
        //delete [] func;
        VirtualFree (func, buffer_.size () + 1, MEM_DECOMMIT);
        //ErrorPrintfBox ("C %x %x\n", this, &buffer_);
        func = nullptr;
        buffer_.clear ();
    }

    size_t Size ()
    {
        return buffer_.size ();
    }
};
#endif // MCODE_H_INCLUDED
