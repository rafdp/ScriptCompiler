#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

uint8_t BuildModRM (const uint8_t mod, uint8_t destination, uint8_t source)
{
    return ((mod << 6) |
            (source << 3) |
            (destination));
}

uint8_t BuildSIB (const uint8_t scale, uint8_t destination, uint8_t source)
{
    return ((scale << 6) |
            (source << 3) |
            (destination));
}

struct Instruction_t
{
    std::vector<uint8_t> opcode;
    Instruction_t () :
        opcode ()
    {}

    Instruction_t (std::initializer_list<uint8_t> list) :
        opcode ()
    {
        Fill (list);
    }

    void Fill (std::initializer_list<uint8_t> list)
    {
        STL_LOOP (it, (list))
            opcode.push_back (*it);
    }
};

class CmdEmitter_t
{
    public:
    MCode_t mcode_;

    CmdEmitter_t () :
        mcode_ ()
    {}

    void EmitOpcode(Instruction_t* instr)
    {
        STL_LOOP (it, instr->opcode)
        {
            mcode_.EmitData (*it);
        }
    }
    template <typename T>
    void EmitData(T data)
    {
        mcode_.EmitData (data);
    }

    void EmitInstruction(Instruction_t instr, uint8_t regDest, uint8_t regSrcOrRmData)
    {
        EmitOpcode (&instr);
        EmitData (BuildModRM(MODE_REGISTER, regDest, regSrcOrRmData));
    }

    void EmitInstruction(Instruction_t instr, uint8_t RMMode, uint8_t reg1, uint8_t reg2)
    {
        EmitOpcode (&instr);
        EmitData (BuildModRM(RMMode, reg1, reg2));
    }

    template <typename T>
    void EmitInstructionImm(Instruction_t instr, uint8_t regDest, T imm)
    {
        EmitOpcode (&instr);
        mcode_.AddToTop (regDest);
        EmitData (imm);
    }

    template <typename T>
    void EmitInstruction(Instruction_t instr, T imm)
    {
        EmitOpcode (&instr);
        EmitData ((int32_t)imm);
    }

    void EmitInstruction(Instruction_t instr)
    {
        EmitOpcode (&instr);
    }
};

class InstructionManager_t
{public:
    CmdEmitter_t emitter_;
    Instruction_t inMov_RM_Imm,
                  inMov_RM_RM,
                  inPush_Imm,
                  inCall_RM,
                  inAdd_RM_Imm,
                  inRetn;

public:
    InstructionManager_t () :
        emitter_      (),
        inMov_RM_Imm  ({0xB8}),
        inMov_RM_RM   ({0x89}),
        inPush_Imm    ({0x68}),
        inCall_RM     ({0xFF}),
        inAdd_RM_Imm  ({0x81}),
        inRetn        ({0xC3})
    {}

    void EmitMov (uint8_t regDest, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inMov_RM_RM, regDest, regSrc);
    }

    template <typename T>
    void EmitMov (uint8_t regDest, T imm)
    {
        emitter_.EmitInstructionImm (inMov_RM_Imm, regDest, imm);
    }

    template <typename T>
    void EmitMov (T* pointer, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inMov_RM_RM, MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData((uint32_t) pointer);
    }

    template <typename T>
    void EmitPush (T imm)
    {
        emitter_.EmitInstruction (inPush_Imm, imm);
    }

    void EmitCall (uint8_t regDest)
    {
        emitter_.EmitInstruction (inCall_RM, regDest, 2);
    }

    template <typename T>
    void EmitAdd (uint8_t regDest, T data)
    {
        emitter_.EmitInstruction (inAdd_RM_Imm, regDest, 0);
        emitter_.EmitData((int32_t) data);
    }

    void EmitRetn ()
    {
        emitter_.EmitInstruction (inRetn);
    }

    void SaveData ()
    {
        FILE* f = fopen ("Jit/jittest.bin", "wb");
        fwrite (emitter_.mcode_.buffer_.data(), 1, emitter_.mcode_.buffer_.size(), f);
        fclose (f);
    }

    void BuildAndRun ()
    {
        emitter_.mcode_.BuildAndRun();
    }

};
#endif // INSTRUCTION_H_INCLUDED
