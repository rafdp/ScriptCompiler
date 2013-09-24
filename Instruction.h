#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

struct Instruction_t
{
    std::vector<uint8_t> opcode;
    Instruction_t () :
        opcode ()
    {}

    Instruction_t (std::initializer_list<uint8_t>* list) :
        opcode ()
    {
        Fill (list);
    }

    void Fill (std::initializer_list<uint8_t>* list)
    {
        STL_LOOP (it, (*list))
            opcode.push_back (*it);
    }
};

class CmdEmitter_t
{
    public:
    MCode_t* mcode_;
    std::map <DWORD, Instruction_t> instructions_;

    Instruction_t inMov

    CmdEmitter_t (Mcode_t* mcode) :
        mcode_ (mcode)
    {}

    bool RegisterInstruction (DWORD cmdCode,
                              const std::initializer_list<uint8_t>& list)
    {
        if (instructions_.find (cmdCode) != instructions_.end())
            return false;
        Instruction_t instr (&list);
        instructions_[cmdCode] = instr;
        return true;
    }

    bool EmitFunction (DWORD cmdCode)
    {
        auto foundInstruction = instructions_.end();
        if ((foundInstruction = instructions_.find (cmdCode)) == instructions_.end())
            return false;

        STL_LOOP (it, foundInstruction->opcode)
        {
            mcode_.EmitData (*it);
        }
        return true;
    }

    void Fill ()
    {
        #define REG_FUNC(name, code) \
        RegisterInstruction (name, code);

        REG_FUNC (ASM_PUSH_IMM, {0x68})
        REG_FUNC (ASM_PUSH_M, {0x68})
    }
};
#endif // INSTRUCTION_H_INCLUDED
