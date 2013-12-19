#ifndef SCRIPTLINE_H_INCLUDED
#define SCRIPTLINE_H_INCLUDED

struct ScriptLine_t
{
    public:
    std::string cmd;
    std::string param1;
    std::string param2;

    ScriptLine_t (FILE* f);

    ~ScriptLine_t ();
};


ScriptLine_t::ScriptLine_t (FILE* f) :
    cmd (),
    param1 (),
    param2 ()
{
    #define CHECK_CHAR(c) if (c == EOF || c == '\n'/* || c == ';'*/) return;
    if (feof (f)) return;

    char last = 0;
    last = _GetString (f, &cmd, ' ');
    CHECK_CHAR (last)
    last = _GetString (f, &param1, ',');
    CHECK_CHAR (last)
    last = _GetString (f, &param2, EOF);
    #undef CHECK_CHAR
}


ScriptLine_t::~ScriptLine_t ()
{ }
#endif
