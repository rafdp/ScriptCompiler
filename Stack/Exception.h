#ifndef exception_defined

#define exception_defined

#include <exception>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>
#include <new>


class NonCopiable_t
{
    NonCopiable_t (const NonCopiable_t&);
    void operator = (const NonCopiable_t&);
    public:
    NonCopiable_t () {}
};

#define DISABLE_CLASS_COPY(type) \
type (const type&); \
type& operator = (const type&);


#define E_NAT(message, code) (message), (code), __LINE__, __FILE__
#define E_CONS(message, code, pt) (message), (code), (pt), __LINE__, __FILE__

#define NAT_EXCEPTION(data, message, code) \
{ \
    ExceptionHandler* e_nat = new (data) ExceptionHandler (E_NAT ((message), (code))); \
    throw *e_nat; \
}

#define CONS_EXCEPTION(data, message, code, old) \
{ \
    ExceptionHandler* ec = new (data) ExceptionHandler (E_CONS ((message), (code), (old).pt_)); \
    throw *ec; \
}

class exception_data;

class ExceptionHandler
{
private:
    const char* message_;
    int error_code_;
    int line_;
    const char* file_;

    const ExceptionHandler* cause_;


public:
    const ExceptionHandler* pt_;
    ExceptionHandler ();
    ExceptionHandler (const char*     message,
                      int             error_code_,
                      int             line,
                      const char*     file);

    ExceptionHandler (const char*             message,
                      int                     error_code_,
                      const ExceptionHandler* cause,
                      int                     line,
                      const char*             file);

    ExceptionHandler (const ExceptionHandler& that);
    ExceptionHandler& operator = (const ExceptionHandler& that);

    ~ExceptionHandler ();
    void WriteLog(exception_data* data) const;
    void* operator new(size_t s, exception_data* data);
};

class exception_data
{
    DISABLE_CLASS_COPY (exception_data)
    public:
        ExceptionHandler* allocatedMem_;
        int64_t usedMem_;
        int64_t availableMem_;
        std::string filename_;
        FILE* log_;

    void* mem_alloc (size_t size = 0)
    {
        if (size > 0)
        {
            allocatedMem_ = new ExceptionHandler [size];
            usedMem_ = 0;
            availableMem_ = static_cast<int64_t> (size);
        }
        else
        {
            if (allocatedMem_ != nullptr)
            {
                if (availableMem_ == 0)
                {
                    printf ("Not enough memory for exceptions\n");
                    return nullptr;
                }
                usedMem_++;
                availableMem_--;

                return reinterpret_cast<void*>(allocatedMem_ + usedMem_ - 1);
            }
            else
            {
                printf ("Memory for exceptions has not been allocated\n");
            }
        }
        return nullptr;
    }

    exception_data (size_t size, const char* filename) :
        allocatedMem_ (nullptr),
        usedMem_      (0),
        availableMem_ (0),
        filename_     (filename),
        log_          (nullptr)
    {
        mem_alloc (size);
    }

    ~exception_data ()
    {
        CloseLog ();
        if (allocatedMem_ != nullptr)
        {
            delete[] allocatedMem_;
            allocatedMem_ = nullptr;
        }
        usedMem_ = 0;
        availableMem_ = 0;
    }

    void OpenLog ()
    {
        if (log_) fclose (log_);
        log_ = fopen (filename_.c_str(), "w");
    }

    void CloseLog ()
    {
        if (log_) fclose (log_);
        log_ = nullptr;
    }

};

ExceptionHandler::ExceptionHandler (const ExceptionHandler& that) :
    message_ (that.message_),
    error_code_ (that.error_code_),
    line_   (that.line_),
    file_   (that.file_),
    cause_     (that.cause_),
    pt_ (that.pt_)
{
}

ExceptionHandler& ExceptionHandler::operator = (const ExceptionHandler& that)
{
    message_    = that.message_;
    error_code_ = that.error_code_;
    line_       = that.line_;
    file_       = that.file_;
    cause_      = that.cause_;
    pt_         = that.pt_;
    return *this;
}

ExceptionHandler::ExceptionHandler () :
    message_ (""),
    error_code_ (0),
    line_   (0),
    file_   (""),
    cause_     (nullptr),
    pt_ (this)
{
}

ExceptionHandler::ExceptionHandler (const char*     message,
                                    int             error_code,
                                    int             line,
                                    const char*     file):
    message_    (message),
    error_code_ (error_code),
    line_       (line),
    file_       (file),
    cause_      (nullptr),
    pt_         (this)
{
}

ExceptionHandler::ExceptionHandler (const char*             message,
                                    int                     error_code,
                                    const ExceptionHandler* cause,
                                    int                     line,
                                    const char*             file):
    message_    (message),
    error_code_ (error_code),
    line_       (line),
    file_       (file),
    cause_      (cause),
    pt_         (this)
{}

ExceptionHandler::~ExceptionHandler ()
{
}

void ExceptionHandler::WriteLog(exception_data* data) const
{
    if (!data->log_)
    {
        //printf ("Log file not initialised\n");
        data->OpenLog ();
    }
    fprintf (data->log_, "Exception with error code %d,\n", error_code_);
    fprintf (data->log_, "error message: \"%s\"\n", message_);
    fprintf (data->log_, "occurred in file: \"%s\" on line %d\n", file_, line_);
    if (cause_ != nullptr)
    {
        fprintf (data->log_, "caused by:\n");
        cause_->WriteLog (data);
    }
    if (data->log_) data->CloseLog ();
}

void* ExceptionHandler::operator new (size_t, exception_data* data)
{
    return data->mem_alloc();
}

#endif


