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
    #define CHECK_int8_t(c) if (c == EOF || c == '\n'/* || c == ';'*/) return;
    if (feof (f)) return;

    int8_t last = 0;
    last = _GetString (f, &cmd, ' ');
    CHECK_int8_t (last)
    last = _GetString (f, &param1, ',');
    CHECK_int8_t (last)
    last = _GetString (f, &param2, EOF);
    #undef CHECK_int8_t
}


ScriptLine_t::~ScriptLine_t ()
{ }
#endif
