
#include "Builder.h"

int main ()
{
    try
    {

    exception_data ex_data (5, "errors.txt");
    ScriptCompiler_t comp ("test.txt", &ex_data);
    comp.Save ();
    comp.Dump ();
    }
    catch (...)
    {printf ("Exception\n");}
}
