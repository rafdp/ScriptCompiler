
#include "Builder.h"

#include "Compiler.h"

void printHello (RunInstanceDataHandler_t*, void* pt)
{
    printf ("__Hello %d__\n", *(int*)pt);
}


int main ()
{
    exception_data ex_data (100, "errors.txt");
    try
    {
        //ScriptCompiler_t comp ("Equation.txt", &ex_data);
        ScriptCompiler_t comp ("test.txt", &ex_data);
        comp.Save ();
        VirtualProcessor_t proc (&ex_data);
        int data = 17;
        proc.RegFunc (UserFunc_t (printHello, &data), "printhello");
        //proc.RunScriptJit ("Equation.pcs");
        proc.RunScript ("test.pcs");
        //proc.RunScript ("Equation.pcs");
        //proc.RunScriptJit ("test.pcs");
        proc.RunScript ("Equation.pcs");
    }
    catch (ExceptionHandler& ex)
    {
        printf ("Exception occurred\nCheck \"errors.txt\"");
        ex.WriteLog (&ex_data);
    }
    catch (std::logic_error err)
    {
        printf ("Exception occurred: %s\n", err.what ());
    }
}

/**
struct YO
  var nigga, type_qword
struct_end

struct NEW
  var vara, type_dword
  var yo, type_YO
  var varw, type_word

  func Init
    mov vara, 667
    push vara
    mov yo.nigga 100500
    push yo.nigga
    mov varw, 2
    push varw
  ret

  func Leave
    mov vara, 0
    mov varw, 0
    mov yo.nigga, 0
    push vara
    push yo.nigga
    push varw
  ret

struct_end

var newvar, type_NEW

call  newvar.Init

push newvar.yo.nigga

print PRINT_STRING, "\n"

call  newvar.Leave
**/
