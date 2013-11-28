#ifndef mem_defined

#define mem_defined

#include "Exception.h"
#include <string>

enum ErrorsDynamicMem
{
    Error_NoMemOnConstruction = 3,
    Error_NoMemOnAllocation,
    Error_NoMemOnDeallocation,
    Error_ThisNullPtr,
    Error_OverflowOccurred,
    Error_InvalidSize,
    Error_DataNullPtrValidSize,
    Error_InFunc,
    Error_UnknownError
};

#define CATCH_IN_FUNC_ERROR(data) \
catch (ExceptionHandler& ex) \
{ \
    static std::string error ("Error occured in "); \
    error += __PRETTY_FUNCTION__; \
    CONS_EXCEPTION (data, error.c_str(), Error_InFunc, ex)\
}

#define CATCH_UNKNOWN_ERROR(data) \
catch (...) \
{ \
    static std::string error ("Unknown error occured in "); \
    error += __PRETTY_FUNCTION__; \
    NAT_EXCEPTION (data, error.c_str(), Error_InFunc) \
}

const size_t MaxBytes = 100 * 1024 * sizeof (int);

template <typename T, size_t Size = 1>
struct DynamicMem
{
protected:
    size_t currSize;
    char* data;
    char* temp;
    const size_t maxElements;
    exception_data* expn;
public:
    void ok () throw (ExceptionHandler&)
    {
        if (!this)
            NAT_EXCEPTION (expn, "This = nullptr", Error_ThisNullPtr)
        if (currSize > maxElements)
        {
            static std::string error ("Overflow occured, max size = ");
            error += std::to_string (MaxBytes);
            error += " bytes";
            NAT_EXCEPTION (expn, error.c_str(), Error_OverflowOccurred)
        }
        if (currSize < 0)
            NAT_EXCEPTION (expn, "currSize < 0", Error_InvalidSize)
        if (!data && currSize != 0)
            NAT_EXCEPTION (expn, "Data = nullptr, currSize != 0", Error_DataNullPtrValidSize)
    }

    DynamicMem (exception_data* ex) :
        currSize (Size),
        data (NULL),
        temp (NULL),
        maxElements (MaxBytes / sizeof(T)),
        expn (ex)
    {
        try
        {
            data = new char [currSize * sizeof (T)];
            ZeroMemory (data, currSize * sizeof (T));
            ok ();

        }
        catch (std::bad_alloc)
        {
            NAT_EXCEPTION (expn, "DynamicMem::DynamicMem (exception_data* ex) Failed to allocate space on heap", Error_NoMemOnAllocation)
        }
        CATCH_IN_FUNC_ERROR (expn)
        CATCH_UNKNOWN_ERROR (expn)
    }

    virtual ~DynamicMem ()
    {
        try
        {
            ok ();
        }
        CATCH_IN_FUNC_ERROR (expn)
        CATCH_UNKNOWN_ERROR (expn)
        delete[] data;
        expn = NULL;
    }

    void Copy (size_t n)
    {
        try
        {
            ok ();
            if (n == 0) return;
            if (temp == NULL) return;
            if (data == NULL) return;
            for (size_t i = 0; i < n; i++) data[i] = temp[i];
            ok ();
        }
        CATCH_IN_FUNC_ERROR (expn)
        CATCH_UNKNOWN_ERROR (expn)
    }
    void GetMem (size_t size)
    {

        if (size <= 0) return;
        if (currSize == 0 && !data)
        {
            try
            {
                ok ();
                data = new char [size * sizeof (T)];
                ZeroMemory (data, size);
                currSize = size;
                ok ();
            }
            catch (std::bad_alloc)
            {
                NAT_EXCEPTION (expn, "DynamicMem::GetMem (size_t) Failed to allocate space on heap",Error_NoMemOnAllocation)
            }
            CATCH_IN_FUNC_ERROR (expn)
            CATCH_UNKNOWN_ERROR (expn)
        }
        else
        {
            try
            {
                ok ();
                temp = data;
                data = new char [(size + currSize) * sizeof (T)];
                ZeroMemory (data, currSize + size);
                Copy (currSize * sizeof (T));
                currSize += size;
                delete[] temp;
                temp = NULL;
                ok ();
            }
            catch (std::bad_alloc)
            {
                NAT_EXCEPTION (expn, "DynamicMem::GetMem (size_t) Failed to allocate space on heap (prev_size != 0)", Error_NoMemOnDeallocation)
            }
            CATCH_IN_FUNC_ERROR (expn)
            CATCH_UNKNOWN_ERROR (expn)
        }

    }
    void FreeMem (int size = -1)
    {
        try
        {
            ok ();

            if (size >= currSize || size == -1)
            {
                delete[] data;
                data = NULL;
                currSize = 0;
            }
            else if (size <= 0) return;
            else
            {
                temp = data;
                data = new char [(currSize - size) * sizeof (T)];
                Copy ((currSize - size) * sizeof (T));
                currSize -= size;
                delete[] temp;
                temp = NULL;
            }

            ok ();

        }
        catch (std::bad_alloc)
        {
            NAT_EXCEPTION (expn, "DynamicMem::FreeMem (size_t) Failed to allocate space on heap while reserving a smaller block of memory", Error_NoMemOnDeallocation)
        }
        CATCH_IN_FUNC_ERROR (expn)
        CATCH_UNKNOWN_ERROR (expn)
    }
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

