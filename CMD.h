
#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED
struct Cmd_t
{
    public:
    char flag;
    long long cmd;
    Cmd_t (std::string str);
    Cmd_t ();

    void Clear ();
    void Error ();
    void Set (char fl, long long cm);
};

Cmd_t::Cmd_t () :
    flag (CMD_Null),
    cmd  (0)
{}

Cmd_t::Cmd_t (std::string str) :
    flag (CMD_Null),
    cmd  (0)
{
    if (!str.size ()) return;
    long long cmd_result = CMD_A_service[str];
    if (cmd_result)
    {
        flag = (char) cmd_result;
        return;
    }
    if (*(str.rbegin ()) == ':')
    {
        cmd = (long long) new std::string (str);
        ((std::string*)cmd)->erase (( (std::string*)cmd)->end () - 1);
        flag = CMD_Label;
        if (CMD_A[ (char*)cmd]) Clear ();
        return;
    }

    cmd_result = CMD_A[str];
    if (cmd_result)
    {
        flag = CMD_Func;
        cmd = cmd_result;
        return;
    }

    {
        cmd = (long long) new std::string (str);
        flag = CMD_Name;
        return;
    }
}

void Cmd_t::Clear ()
{
    if (flag == CMD_Label || flag == CMD_Name)
    {
        delete (std::string*)cmd;
    }
    flag = CMD_Null;
    cmd = 0;
}

void Cmd_t::Error ()
{
    Clear ();
    flag = CMD_Error;
}


void Cmd_t::Set (char fl, long long cm)
{
    flag = fl;
    cmd = cm;
}

#endif
