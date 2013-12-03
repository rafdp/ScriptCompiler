
#define $ VirtualProcessor_t::currentlyExecuting_->instance_->

void OnSigArgSwitch (std::string* error, char* flag, int64_t* arg)
{
    switch (*flag & ~char (ARG_UNREF_MASK))
    {
        case ARG_LABEL:
        case ARG_FUNC:
            *error += "pointing to line ";
        case ARG_NUM:
            *error += std::to_string (*arg);
            *error += "\n";
            break;
        case ARG_REG:
            *error += CMD_REG_D[*arg];
            *error += " contains ";
            *error += std::to_string ($ GetReg (*arg) . Get8byte ());
            *error += "\n";
            break;
        case ARG_VAR:
        case ARG_VAR_MEMBER:
        {
            *error += "on address ";
            void* ptr = $ GetPtr (*flag, *arg);
            *error += std::to_string ((int32_t)ptr);
            if (ptr)
            {
                *error += " contains ";
                *error += std::to_string ($ GetVal (*flag, *arg));
            }
            *error += "\n";
            break;
        }
        case ARG_STR:
            *error += "\"";
            *error += $ strings_[*arg];
            *error += "\"\n";
            break;
        case ARG_FUNC_MEMBER:
            *error += "pointing to line ";
            *error += std::to_string (int (*arg));
            *error += "\n";
            break;
        case ARG_DLL_FUNC:
            *error += "on address ";
            *error += std::to_string ((int32_t)$ dllResolved_[*arg]);
            *error += "\n";
            break;
        default:
            *error += "\n";
    }
}

void OnSigEmitInfo (std::string* error)
{
    *error += "Last executed line = ";
    *error += std::to_string ($ run_line_);
    *error += ";\n";
    auto found = CMD_D.find ($ funcs_[$ run_line_].cmd);
    if (found != CMD_D.end ())
    {
        *error += "Cmd: ";
        *error += found->second;
        *error += "\nArg1: ";
        *error += ARG_D[$ args_[$ run_line_].flag1];
        *error += " ";
        OnSigArgSwitch (error, & $ args_[$ run_line_].flag1, & $ args_[$ run_line_].arg1);
        *error += "Arg2: ";
        *error += ARG_D[$ args_[$ run_line_].flag2];
        *error += " ";
        OnSigArgSwitch (error, & $ args_[$ run_line_].flag2, & $ args_[$ run_line_].arg2);
    }
}

void OnSigAbort (int)
{
    static std::string error;
    if (VirtualProcessor_t::currentlyExecuting_)
    {
        error += "\nAbort signal occurred:\n";
        OnSigEmitInfo (&error);
        NAT_EXCEPTION (VirtualProcessor_t::currentlyExecuting_->expn_, error.c_str(), ERROR_SIGNAL_ABORT)
    }
    else
    {
        error += "Abort signal occurred";
        throw std::logic_error (error);
    }

}

void OnSigFpe (int)
{
    static std::string error;
    if (VirtualProcessor_t::currentlyExecuting_)
    {
        error += "\nFPE signal occurred (Erroneous arithmetic operation, such as zero divide):\n";
        OnSigEmitInfo (&error);
        NAT_EXCEPTION (VirtualProcessor_t::currentlyExecuting_->expn_, error.c_str(), ERROR_SIGNAL_FPE)
    }
    else
    {
        error += "FPE signal occurred (Erroneous arithmetic operation, such as zero divide)";
        throw std::logic_error (error);
    }
}

void OnSigIll (int)
{
    static std::string error;
    if (VirtualProcessor_t::currentlyExecuting_)
    {
        error += "\nIllegal instruction signal occurred:\n";
        OnSigEmitInfo (&error);
        NAT_EXCEPTION (VirtualProcessor_t::currentlyExecuting_->expn_, error.c_str(), ERROR_SIGNAL_ILL)
    }
    else
    {
        error += "Illegal instruction signal occurred";
        throw std::logic_error (error);
    }
}

void OnSigInt (int)
{
    static std::string error;
    if (VirtualProcessor_t::currentlyExecuting_)
    {
        error += "\nInterruption occurred:\n";
        OnSigEmitInfo (&error);
        NAT_EXCEPTION (VirtualProcessor_t::currentlyExecuting_->expn_, error.c_str(), ERROR_SIGNAL_INT)
    }
    else
    {
        error += "Interruption occurred";
        throw std::logic_error (error);
    }
}

void OnSigSegv (int)
{
    static std::string error;
    if (VirtualProcessor_t::currentlyExecuting_)
    {
        error += "\nSegmentation violation:\n";
        OnSigEmitInfo (&error);
        NAT_EXCEPTION (VirtualProcessor_t::currentlyExecuting_->expn_, error.c_str(), ERROR_SIGNAL_SEGV)
    }
    else
    {
        error += "Segmentation violation";
        throw std::logic_error (error);
    }
}

void OnSigTerm (int)
{
    static std::string error;
    if (VirtualProcessor_t::currentlyExecuting_)
    {
        error += "\nTermination signal occurred:\n";
        OnSigEmitInfo (&error);
        NAT_EXCEPTION (VirtualProcessor_t::currentlyExecuting_->expn_, error.c_str(), ERROR_SIGNAL_TERM)
    }
    else
    {
        error += "Termination signal occurred";
        throw std::logic_error (error);
    }
}

void RegisterSignalHandlers ()
{
    signal (SIGABRT, OnSigAbort);
    signal (SIGFPE,  OnSigFpe);
    signal (SIGILL,  OnSigIll);
    signal (SIGINT,  OnSigInt);
    signal (SIGSEGV, OnSigSegv);
    signal (SIGTERM, OnSigTerm);
}

#undef $
