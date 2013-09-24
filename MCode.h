#ifndef MCODE_H_INCLUDED
#define MCODE_H_INCLUDED

class MCode_t
{
    std::vector <unsigned char> buffer_;

    public:
    MCode () :
        buffer_ ()
    {}

    template <typename T>
    void EmitData (T val)
    {
        for (int i = 0; i < sizeof (T); i++)
        {
            buffer_.push_back (uint8_t (val >> i * 8));
        }
    }
};
#endif // MCODE_H_INCLUDED
