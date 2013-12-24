#ifndef ARG_H_INCLUDED
#define ARG_H_INCLUDED

struct Arg_t;

void SetStringBadArg (std::string* errorMessage, Arg_t arg_got, ExpectedArg_t expArg);

struct Arg_t
{
    public:
    int8_t flag1;
    int8_t flag2;
    int64_t arg1;
    int64_t arg2;

    Arg_t ();

    Arg_t (std::string param1, std::string param2);

    void SetArg (int8_t* flag, int64_t* arg, std::string* param);

    void FreeArg (int8_t* flag, int64_t* arg);

    void Clear ();

    void Set (int8_t fl1, int64_t ar1, int8_t fl2, int64_t ar2);
};

Arg_t::Arg_t () :
    flag1 (0),
    flag2 (0),
    arg1  (0),
    arg2  (0)
{}


Arg_t::Arg_t (std::string param1, std::string param2) :
    flag1 (0),
    flag2 (0),
    arg1  (0),
    arg2  (0)
{
    SetArg (&flag1, &arg1, &param1);
    SetArg (&flag2, &arg2, &param2);
}

void Arg_t::SetArg (int8_t* flag, int64_t* arg, std::string* param)
{
    *flag = 0;

    if (param->empty ())
    {
        *flag |= ARG_NULL;
        *arg = 0;
        return;
    }

    if ( (*param)[0] == '*') {*flag |= static_cast<int8_t> (ARG_UNREF_MASK); param->erase (0, 1);}

    auto reg = CMD_REG.find (*param);
    if (reg != CMD_REG.end ())
    {
        *flag |= ARG_REG;
        *arg = reg->second;
        return;
    }
    else
    if (isNum (param))
    {
        *flag |= ARG_NUM;
        *arg = atoll (param->c_str ());
        return;
    }
    else
    {
        *flag |= ARG_NAME;
        *arg = reinterpret_cast<int64_t> (new std::string (*param));
    }
}

void Arg_t::FreeArg (int8_t* flag, int64_t* arg)
{
    if (*flag == ARG_NAME)
        delete reinterpret_cast<std::string*> (*arg);
    *flag = ARG_NULL;
    *arg = 0;
}

void Arg_t::Clear ()
{
    FreeArg (&flag1, &arg1);
    FreeArg (&flag2, &arg2);
}

void Arg_t::Set (int8_t fl1, int64_t ar1, int8_t fl2, int64_t ar2)
{
    flag1 = fl1;
    arg1 = ar1;
    flag2 = fl2;
    arg2 = ar2;
}

void SetStringBadArg (std::string* errorMessage, Arg_t arg_got, ExpectedArg_t expArg)
{
    const int LEN = 15;
    *errorMessage += "Bad argument type:\nexpected:\n";

    size_t minSize = (expArg.expFlag1.size () < expArg.expFlag2.size ()) ? expArg.expFlag1.size () : expArg.expFlag2.size ();
    int max_num = 1;
    size_t maxSize = (minSize == expArg.expFlag1.size ()) ? expArg.expFlag2.size () : expArg.expFlag1.size ();
    if (maxSize == expArg.expFlag2.size ()) max_num++;
    for (size_t i = 0; i < minSize; i++)
    {
        *errorMessage += "    " + ARG_D[expArg.expFlag1[i]];
        for (size_t j = 0; j < LEN - ARG_D[expArg.expFlag1[i]].size (); j++) *errorMessage += ' ';
        *errorMessage += ARG_D[expArg.expFlag2[i]];
        *errorMessage += '\n';
    }
    for (size_t i = 0; i < maxSize - minSize; i++)
    {
        if (max_num == 1)
        {
            *errorMessage += "    " + ARG_D[expArg.expFlag1[i + minSize]];
        }
        else
        {
            for (int j = 0; j < LEN + 4; j++) *errorMessage += ' ';
            *errorMessage += ARG_D[expArg.expFlag2[i + minSize]];
        }
        *errorMessage += '\n';
    }

    *errorMessage += std::string ("received:\n    ") + ARG_D[arg_got.flag1];
    int arg1size = static_cast<int> (ARG_D[arg_got.flag1].size ());
    for (int i = 0; i < LEN - arg1size; i++) *errorMessage += ' ';
    *errorMessage += ARG_D[arg_got.flag2];
}

struct ManageInputArgs_t
{
    ExpectedArg_t expArgs;
    Arg_t argsGot;
    bool error;
    ManageInputArgs_t (const ExpectedArg_t& expArgs_, Arg_t argsGot_) :
        expArgs (expArgs_),
        argsGot (argsGot_),
        error (false)
    {
        if (find (expArgs.expFlag1, int8_t (argsGot.flag1 & ~int8_t (ARG_UNREF_MASK))) == expArgs.expFlag1.end () ||
            find (expArgs.expFlag2, int8_t (argsGot.flag2 & ~int8_t (ARG_UNREF_MASK))) == expArgs.expFlag2.end ())
            error = true;
    }

    ErrorReturn_t CreateError (int val = RET_ERROR_CONTINUE, std::string error_ = std::string ())
    {
        if (error_.empty ())
            SetStringBadArg (&error_, argsGot, expArgs);
        return ErrorReturn_t (val, error_);
    }

};

#endif
