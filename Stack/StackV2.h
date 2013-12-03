#ifndef stack_defined

#define stack_defined

#include "DynamicArray.cpp"

enum ErrorsStack
{
    Error_AccessLow,
    Error_AccessHigh,
    Error_InvalidVector,
    Error_InvalidStack,
    Error_StackNullPtr
};

template <typename T>
class stack
{
public:
    Array<T> data;
    void ok () throw (ExceptionHandler&);
    stack(exception_data* ex);
    virtual ~stack();

    exception_data* expn;

    class iterator;
    void push (const T& n);
    T pop ();
    bool empty ();
    int size ();

    T& top();
    int topIndex();

    T& operator[] (int index);
    T& operator[] (iterator iter);
    T& At (int index);
    T& At (iterator iter);

    void Dump ();

    iterator begin ()
    {
        iterator iter (this);
        return iter;
    }

    iterator end ()
    {
        iterator iter (this, data.right() + 1);
        return iter;
    }
};
template <typename T>
void stack<T>::ok () throw (ExceptionHandler&)
{
    if (!this)
    {
        NAT_EXCEPTION (expn, "This = nullptr", Error_ThisNullPtr)
    }
    try
    {
        data.ok ();
    }
    catch (ExceptionHandler& e)
    {
        CONS_EXCEPTION (expn, "Error in vector occured", Error_InvalidVector, e)
    }
}

template <typename T>
stack<T>::stack(exception_data* exp)   //default ctor
try :
    data (exp),
    expn (exp)
{
    ok ();
}
CATCH_IN_FUNC_ERROR (expn)
CATCH_UNKNOWN_ERROR (expn)
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
void stack<T>::Dump ()
{
    if (data.IsEmpty())
    {
        printf ("stack empty\n");
        return;
    }
    printf ("stack:\n");
    iterator iter (this);
    for ( ; iter < end(); ++iter)
        {std::cout << "  [" << iter.GetIndex() << "] = " << *iter << std::endl;}
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
stack<T>::~stack ()
{}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
/*
void* operator new (size_t size, void* mem)
{
    return mem;
}
*/
template <typename T>
void stack<T>::push (const T& n)
{
    try
    {
        ok ();
        data.push_back(n);
        ok ();
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
T stack<T>::pop ()
{
    try
    {
        ok ();
        T ret = data.pop();
        ok ();
        return ret;
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
bool stack<T>::empty ()
{
    try
    {
        ok ();
        return data.IsEmpty();
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
int stack<T>::size ()
{
    try
    {
        ok ();
        return data.GetSize();
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
T& stack<T>::top()
{
    try
    {
        ok ();
        return *(data.right());
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
int stack<T>::topIndex()
{
    try
    {
        ok ();
        return data.GetSize() - 1;
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
T& stack<T>::operator[] (int index)
{
    try
    {
        ok ();
        return data[index];
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
T& stack<T>::operator[] (iterator iter)
{
    try
    {
        ok ();
        return data[iter.GetIndex()];
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
T& stack<T>::At (int index)
{
    try
    {
        ok ();
        return data.At(index);
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
T& stack<T>::At (iterator iter)
{
    try
    {
        ok ();
        return data.At(iter.GetIndex());
    }
    CATCH_IN_FUNC_ERROR (expn)
    CATCH_UNKNOWN_ERROR (expn)
}
//!}--------------------------------------------------------------------
//!}--------------------------------------------------------------------
template <typename T>
class stack<T>::iterator
{
private:
    stack<T>* st_;
    T* currPos_;

public:

    void ok ()
    {
        if (!this)
        {
            NAT_EXCEPTION (st_->expn, "This = nullptr", Error_ThisNullPtr)
        }
        try
        {
            st_->ok ();
        }
        catch (ExceptionHandler& e)
        {
            CONS_EXCEPTION (st_->expn, "Error in stack occured", Error_InvalidStack, e)
        }
        bool lowErr  = (currPos_ < st_->data.left());
        bool highErr = (currPos_ > st_->data.right() + 1);
        if (lowErr)
        {
            NAT_EXCEPTION (st_->expn, "Access violation: too low iterator position", Error_AccessLow)
        }
        if (highErr)
        {
            NAT_EXCEPTION (st_->expn, "Access violation: too high iterator position", Error_AccessHigh)
        }
    }

    iterator (stack<T>* st) :
            st_        (st),
            currPos_ (st_->data.left())

    {
        try
        {
            st_->ok ();
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }

    iterator (stack<T>* st, T* pt) :
            st_        (st),
            currPos_ (pt)
    {
        try
        {
            st_->ok ();
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }

    //!////////////////////////////////////////////////////////
    iterator& operator++ ()
    {
        try
        {
            ok();
            ++currPos_;
            ok();
            return *this;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    iterator& operator++ (int)
    {
        try
        {
            ok();
            currPos_++;
            ok();
            return *this;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    iterator& operator-- ()
    {
        try
        {
            ok();
            --currPos_;
            ok();
            return *this;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    iterator& operator-- (int)
    {
        try
        {
            ok();
            currPos_--;
            ok();
            return *this;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    bool operator > (const iterator& iter)
    {
        try
        {
            ok();
            return !(*this == iter) && !(*this < iter);
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    bool operator < (const iterator& iter)
    {
        try
        {
            ok();
            return (currPos_ < iter.currPos_);
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    bool operator == (const iterator& iter)
    {
        try
        {
            ok();
            return (currPos_ == iter.currPos_);
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    iterator operator = (const iterator& iter)
    {
        try
        {
            ok();
            currPos_ = iter.currPos_;
            st_ = iter.st_;
            ok();
            return iter;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    T& operator * ()
    {
        try
        {
            ok();
            return *currPos_;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    //!////////////////////////////////////////////////////////
    T& operator -> ()
    {
        try
        {
            ok();
            return &*currPos_;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    int GetIndex ()
    {
        try
        {
            ok();
            return currPos_ - st_->data.left();
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
    //!////////////////////////////////////////////////////////
    T* GetPos ()
    {
        try
        {
            ok();
            return currPos_;
        }
        CATCH_IN_FUNC_ERROR (st_->expn)
        CATCH_UNKNOWN_ERROR (st_->expn)
    }
};

#endif
