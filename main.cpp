
#include "Builder.h"

#include "Compiler.h"

void printHello (RunInstanceDataHandler_t*, void* pt);

void printHello (RunInstanceDataHandler_t*, void* pt)
{
    printf ("__Hello %d__\n", *reinterpret_cast<int*> (pt));
}


int main ()
{
    exception_data ex_data (100, "errors.txt");
    try
    {
        //ScriptCompiler_t comp ("Equation.txt", &ex_data);
        ErrorPrintfBox ("A main");
        ScriptCompiler_t comp ("test.txt", &ex_data);
        ErrorPrintfBox ("B main");
        comp.Save ();
        ErrorPrintfBox ("C main");
        VirtualProcessor_t proc (&ex_data);
        ErrorPrintfBox ("D main");
        int data = 17;
        proc.RegFunc (UserFunc_t (printHello, &data), "printhello");
        ErrorPrintfBox ("E main");
        //proc.RunScriptJit ("Equation.pcs");
        proc.RunScriptJit ("test.pcs");
        ErrorPrintfBox ("F main");
        //proc.RunScript ("Equation.pcs");
        ErrorPrintfBox ("G main");
        //proc.RunScriptJit ("test.pcs");
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

mov D, B
    mul D, B

    var temp, type_qword

    mov temp, A
    mul temp, C
    mul temp, FOUR

    sub D, temp
**/
