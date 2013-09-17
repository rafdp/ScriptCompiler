
#include "Compiler.h"

ErrorReturn_t printHello (RunInstanceDataHandler_t*, Arg_t, void* pt)
{
    printf ("__Hello %d__\n", *(int*)pt);
    return ErrorReturn_t (RET_SUCCESS);
}

int main()
{
    exception_data ex_data (5, "errors.txt");
    try
    {
        VirtualProcessor_t proc (&ex_data);
        int data = 17;
        proc.RegFunc (UserFunc_t(printHello, &data), "printhello");
        proc.RunScript ("test.pcs", MODE_PRINTF);
    }
    catch (ExceptionHandler& e)
    {
        e.WriteLog (&ex_data);
    }
}

