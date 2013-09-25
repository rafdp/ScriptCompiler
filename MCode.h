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
        *(buffer_.rbegin()) += val;
    }


};
#endif // MCODE_H_INCLUDED
