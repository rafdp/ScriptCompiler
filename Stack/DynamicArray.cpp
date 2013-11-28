#include "DynamicArray.h"

enum ErrorsArray
{
    Error_FailedInit = -42,
    Error_FailedPush,
    Error_FailedPop,
    Error_EmptyArray,
    Error_AccessViolation,
    Error_InvalidAllocator
};

template <typename T, int Size, class Allocator>
void Array<T, Size, Allocator>::ok () throw (ExceptionHandler&)
{
    if (!this)
    {
        NAT_EXCEPTION (Allocator::expn, "This = nullptr", Error_ThisNullPtr);
    }
    try
    {
        Allocator::ok ();
    }
    catch (ExceptionHandler& e)
    {
        CONS_EXCEPTION (Allocator::expn, "Error in allocator occurred", Error_InvalidAllocator, e);
    }
}

template <typename T, int Size, class Allocator>
Array<T, Size, Allocator>::Array (exception_data* expn)
try :
    Allocator (expn),
    currSize_ (0)
{
    ok ();
}
CATCH_IN_FUNC_ERROR (Allocator::expn)
CATCH_UNKNOWN_ERROR (Allocator::expn)

template <typename T, int Size, class Allocator>
Array<T, Size, Allocator>::~Array ()
{
    try
    {
        ok ();
        for (size_t i = 0; i < currSize_; i ++)
            reinterpret_cast<T*>((Allocator::data + i * sizeof (T)))->~T();
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
void Array<T, Size, Allocator>::Dump ()
{
    printf ("Array:\n");
    for (int i = 0; i < currSize_; ++i)
        std::cout << "  [" << i << "] = " << Allocator::data[i] << std::endl;
}

template <typename T, int Size, class Allocator>
void Array<T, Size, Allocator>::push_back(const T& elem)
{
    try
    {
        ok ();
        Allocator::GetMem (1);
        new (Allocator::data + currSize_ * sizeof (T)) T (elem);
        currSize_++;
        ok ();
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
void Array<T, Size, Allocator>::push_back(T* elements, size_t size)
{
    try
    {
        ok ();
        Allocator::GetMem (size);
        for (int i = 0; i < size; i ++) new (Allocator::data + (currSize_ + i) * sizeof (T)) T (elements[i]);
        currSize_ += size;
        ok ();
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}


template <typename T, int Size, class Allocator>
T Array<T, Size, Allocator>::pop()
{
    try
    {
        if (currSize_ == 0)
        {
            NAT_EXCEPTION (Allocator::expn, "Cannot delete object: empty array", Error_EmptyArray)
        }
        else
        {
            ok ();
            T obj = Allocator::data[(currSize_ - 1) * sizeof (T)];
            Allocator::FreeMem(1);
            currSize_--;
            ok ();
            return obj;
        }
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
T& Array<T, Size, Allocator>::operator[](int index)
{
    try
    {
        ok ();
        return (T&) (Allocator::data[index * sizeof (T)]);
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
T& Array<T, Size, Allocator>::At(int index)
{
    try
    {
        ok ();
        if (index >= currSize_ || index < 0)
        {
            NAT_EXCEPTION (Allocator::expn, "Access violation: out of array range", Error_AccessViolation)
        }
        else return (T&)(Allocator::data[index * sizeof (T)]);
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)

}

template <typename T, int Size, class Allocator>
T* Array<T, Size, Allocator>::left()
{
    try
    {
        ok ();
        return reinterpret_cast<T*>(Allocator::data);
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
T* Array<T, Size, Allocator>::right()
{
    try
    {
        ok ();
        return reinterpret_cast<T*>(Allocator::data + (currSize_ - 1) * sizeof (T));
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
bool Array<T, Size, Allocator>::IsEmpty()
{
    try
    {
        ok ();
        return (!currSize_);
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}

template <typename T, int Size, class Allocator>
int Array<T, Size, Allocator>::GetSize()
{
    try
    {
        ok ();
        return currSize_;
    }
    CATCH_IN_FUNC_ERROR (Allocator::expn)
    CATCH_UNKNOWN_ERROR (Allocator::expn)
}
