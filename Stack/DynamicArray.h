#ifndef dynArr_defined

#define dynArr_defined

#include "MemoryClass.h"

template <typename T, int Size = 0, class Allocator = DynamicMem <T, Size> >
class Array : private Allocator
{
private:
    size_t currSize_;
public:
    void ok () throw (ExceptionHandler&);
    Array(exception_data* ex);
    virtual ~Array();
    void Dump();
    void push_back(const T& elem);
    void push_back(T* elements, size_t size);
    T pop();
    T& operator[] (int index);
    T& At (int index);
    T* left();
    T* right();
    bool IsEmpty();
    int GetSize();
};

#endif

/*

template <typename T,
          int Size = 0,
          class Allocator <T> = DynamicMem <T, Size>
         >
class vector : private Allocator <T>
{
};


*/

