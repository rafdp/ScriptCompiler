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
            buffer_.push_back ((uint8_t)(reinterpret_cast<int32_t>(val) >> i * 8));

        }
    }

    template <typename T>
    void EmitData (T val)
    {
        for (int i = 0; i < sizeof (T); i++)
        {
            buffer_.push_back (uint8_t (val >> i * 8));
        }
    }

    void AddToTop (uint8_t val)
    {
        //*(buffer_.rbegin ()) += val;
        *(uint8_t*)(buffer_.data () + buffer_.size () - 1) += val;
    }

    void BuildAndRun ()
    {
        unsigned char* func = new unsigned char [buffer_.size () + 1];
        memcpy (func, buffer_.data (), buffer_.size ());
        //ErrorPrintfBox ("%X", func);
        //VirtualProtect (func, buffer_.size (), PAGE_EXECUTE_READWRITE, nullptr);
        //for (size_t i = 0; i < buffer_.size (); i++) printf ("%02X ", buffer_[i]);
        //printf ("\n");
        ((void (*) ())func) ();
        delete [] func;
        func = nullptr;
        buffer_.clear ();
    }

    size_t Size ()
    {
        return buffer_.size ();
    }
};
#endif // MCODE_H_INCLUDED
