#include "Builder.h"

#include "Compiler.h"

void printHello (RunInstanceDataHandler_t*, void* pt);

void printHello (RunInstanceDataHandler_t*, void* pt)
{
    printf ("__Hello %d__\n", *reinterpret_cast<int*> (pt));
}


int main ()
{
    /*int i = 0;
    DebugBreak ();
    scanf ("%d\n", &i);
    DebugBreak ();*/

    exception_data ex_data (100, "errors.txt");
    try
    {
        //ScriptCompiler_t comp ("Equation.txt", &ex_data);
        ScriptCompiler_t comp ("test.txt", &ex_data);
        comp.Save ();
        VirtualProcessor_t proc (&ex_data, MODE_PRINTF);
        int data = 17;
        proc.RegFunc (UserFunc_t (printHello, &data), "printhello");
        //proc.RunScriptJit ("Equation.pcs");
        proc.RunScriptJit ("test.pcs");
        //proc.RunScript ("Equation.pcs");
        //proc.RunScript ("test.pcs");
    }
    catch (ExceptionHandler& ex)
    {
        printf ("Exception occurred\nCheck \"errors.txt\"");
        ex.WriteLog (&ex_data);
    }
    catch (std::exception err)
    {
        printf ("Exception occurred: %s\n", err.what ());
    }
    catch (...)
    {
        printf ("Exception occurred\n");
    }
}

/**
struct YO
  var specific_var, type_qword
struct_end

struct NEW
  var vara, type_dword
  var yo, type_YO
  var varw, type_word

  func Init
    mov vara, 667
    push vara
    mov yo.specific_var 100500
    push yo.specific_var
    mov varw, 2
    push varw
  ret

  func Leave
    mov vara, 0
    mov varw, 0
    mov yo.specific_var, 0
    push vara
    push yo.specific_var
    push varw
  ret

struct_end

var newvar, type_NEW

call  newvar.Init

push newvar.yo.specific_var

print PRINT_STRING, "\n"

call  newvar.Leave

mov D, B
    mul D, B

    var temp, type_qword

    mov temp, A
    mul temp, C
    mul temp, FOUR

    sub D, temp
**/
