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
{
    CmdEmitter_t emitter_;
    Instruction_t inMov_RM_Imm,
                  inMov_RM_R,
                  inMov_R_RM,
                  inPush_Imm,
                  inPush_RM,
                  inCall_RM,
                  inRetn,
                  inAdd_RM_Imm,
                  inAdd_R_RM,
                  inAdd_RM_R,
                  inSub_RM_Imm,
                  inSub_R_RM,
                  inSub_RM_R,
                  inIMul_R_RM_Imm,
                  inIMul_R_RM,
                  inIDiv_RM;

public:
    InstructionManager_t () :
        emitter_         (),
        inMov_RM_Imm     ({0xB8}),
        inMov_RM_R       ({0x89}),
        inMov_R_RM       ({0x8B}),
        inPush_Imm       ({0x68}),
        inPush_RM        ({0xFF}),
        inCall_RM        ({0xFF}),
        inRetn           ({0xC3}),
        inAdd_RM_Imm     ({0x81}),
        inAdd_R_RM       ({0x01}),
        inAdd_RM_R       ({0x03}),
        inSub_RM_Imm     ({0x81}),
        inSub_R_RM       ({0x2B}),
        inSub_RM_R       ({0x29}),
        inIMul_R_RM_Imm  ({0x69}),
        inIMul_R_RM      ({0x0F, 0xAF}),
        inIDiv_RM        ({0xF7})
    {}

    void EmitMov (uint8_t regDest, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inMov_RM_R, regDest, regSrc);
    }

    template <typename T>
    void EmitMov (uint8_t regDest, T imm)
    {
        emitter_.EmitInstructionImm (inMov_RM_Imm, regDest, imm);
    }

    template <typename T>
    void EmitMov (T* pointer, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inMov_RM_R, MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData((uint32_t) pointer);
    }

    template <typename T>
    void EmitMov (uint8_t regDest, T* pointer)
    {
        emitter_.EmitInstruction (inMov_R_RM, MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData((uint32_t) pointer);
    }

    template <typename T>
    void EmitPush (T imm)
    {
        emitter_.EmitInstruction (inPush_Imm, imm);
    }

    template <typename T>
    void EmitPush (T* mem)
    {
        emitter_.EmitInstruction (inPush_RM, MODE_ADDRESS, 6);
        emitter_.EmitData (mem);
    }

    void EmitPush (uint8_t reg)
    {
        emitter_.EmitInstruction (inPush_RM, reg, 6);
    }

    void EmitCall (uint8_t reg)
    {
        emitter_.EmitInstruction (inCall_RM, reg, 2);
    }

    void EmitRetn ()
    {
        emitter_.EmitData((uint8_t) 0xCC);
        emitter_.EmitInstruction (inRetn);
    }

    void SaveData ()
    {
        FILE* f = fopen ("Jit/jittest.bin", "wb");
        fwrite (emitter_.mcode_.buffer_.data(), 1, emitter_.mcode_.buffer_.size(), f);
        fclose (f);
    }

    template <typename T>
    void EmitAdd (uint8_t regDest, T data)
    {
        emitter_.EmitInstruction (inAdd_RM_Imm, regDest, 0);
        emitter_.EmitData((int32_t) data);
    }

    template <typename T>
    void EmitAdd (uint8_t regDest, T* src)
    {
        emitter_.EmitInstruction (inAdd_R_RM, MODE_ADDRESS, regDest, OFF);
        emitter_.EmitData((int32_t) src);
    }

    template <typename T>
    void EmitAdd (T* dest, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inAdd_RM_R, MODE_ADDRESS, regSrc, OFF);
        emitter_.EmitData((int32_t) dest);
    }

    template <typename T>
    void EmitSub (uint8_t regDest, T data)
    {
        emitter_.EmitInstruction (inSub_RM_Imm, regDest, 5);
        emitter_.EmitData((int32_t) data);
    }

    template <typename T>
    void EmitSub (uint8_t regDest, T* src)
    {
        emitter_.EmitInstruction (inSub_R_RM, MODE_ADDRESS, regDest, OFF);
        emitter_.EmitData((int32_t) src);
    }

    template <typename T>
    void EmitSub (T* dest, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inSub_RM_R, MODE_ADDRESS, regSrc, OFF);
        emitter_.EmitData((int32_t) dest);
    }

    template <typename T>
    void EmitMul (uint8_t regDest, uint8_t regSrc, T data)
    {
        emitter_.EmitInstruction (inIMul_R_RM_Imm, regDest, regSrc);
        emitter_.EmitData((int32_t) data);
    }

    template <typename T>
    void EmitMul (uint8_t regDest, T* src)
    {
        emitter_.EmitInstruction (inIMul_R_RM, MODE_ADDRESS, regDest, OFF);
        emitter_.EmitData((int32_t) src);
    }

    void EmitMul (uint8_t regDest, uint8_t regSrc)
    {
        emitter_.EmitInstruction (inIMul_R_RM, regDest, regSrc);
    }

    void EmitDiv (uint8_t regSrc)
    {
        emitter_.EmitInstruction (inIDiv_RM, regSrc, 7);
    }

    template <typename T>
    void EmitDiv (T* ptr)
    {
        emitter_.EmitInstruction (inIDiv_RM, MODE_ADDRESS, OFF, 7);
        emitter_.EmitData((int32_t) ptr);
    }

    void Emit64Prefix ()
    {
        emitter_.EmitData((int8_t) 0x48);
    }

    void BuildAndRun ()
    {
        emitter_.mcode_.BuildAndRun();
    }

};
#endif // INSTRUCTION_H_INCLUDED
