#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

struct CPURegisterInfo_t
{
    int8_t reg;
    explicit CPURegisterInfo_t (int8_t reg_) :
        reg (reg_)
    {}
};

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

    void EmitOpcode (Instruction_t* instr)
    {
        STL_LOOP (it, instr->opcode)
        {
            mcode_.EmitData (*it);
        }
    }
    template <typename T>
    void EmitData (T data)
    {
        mcode_.EmitData (data);
    }

    void EmitInstruction (Instruction_t instr,
                          CPURegisterInfo_t regDest,
                          CPURegisterInfo_t regSrc)
    {
        EmitOpcode (&instr);
        EmitData (BuildModRM (MODE_REGISTER, regDest.reg, regSrc.reg));
    }

    void EmitInstruction (Instruction_t instr,
                          CPURegisterInfo_t regDest,
                          uint8_t rmData)
    {
        EmitOpcode (&instr);
        EmitData (BuildModRM (MODE_REGISTER, regDest.reg, rmData));
    }

    void EmitInstruction (Instruction_t instr,
                          uint8_t RMMode,
                          uint8_t prm1,
                          uint8_t prm2)
    {
        EmitOpcode (&instr);
        EmitData (BuildModRM (RMMode, prm1, prm2));
    }

    void EmitInstruction (Instruction_t instr,
                          uint8_t RMMode,
                          CPURegisterInfo_t reg1,
                          CPURegisterInfo_t reg2)
    {
        EmitInstruction (instr, RMMode, reg1.reg, reg2.reg);
    }

    void EmitInstruction (Instruction_t instr,
                          uint8_t RMMode,
                          uint8_t prm,
                          CPURegisterInfo_t reg)
    {
        EmitInstruction (instr, RMMode, prm, reg.reg);
    }

    void EmitInstruction (Instruction_t instr,
                          uint8_t RMMode,
                          CPURegisterInfo_t reg,
                          uint8_t prm)
    {
        EmitInstruction (instr, RMMode, reg.reg, prm);
    }

    template <typename T>
    void EmitInstructionImmTop (Instruction_t instr, CPURegisterInfo_t regDest, T imm)
    {
        EmitOpcode (&instr);
        mcode_.AddToTop (regDest.reg);
        EmitData (imm);
    }

    template <typename T>
    void EmitInstruction (Instruction_t instr, T imm)
    {
        EmitOpcode (&instr);
        EmitData (imm);
    }

    void EmitInstruction (Instruction_t instr)
    {
        EmitOpcode (&instr);
    }
};

class InstructionManager_t
{
    CmdEmitter_t emitter_;
    Instruction_t inMov_RM_Imm_8,
                  inMov_RM_Imm,
                  inMov_RM_R_8,
                  inMov_RM_R,
                  inMov_R_RM_8,
                  inMov_R_RM,
                  inPush_Imm_8,
                  inPush_Imm,
                  inPush_RM,
                  inPop_RM,
                  inCall_RM,
                  inCall_Rel,
                  inRetn,
                  inAdd_RM_Imm_8,
                  inAdd_RM_ImmF,
                  inAdd_RM_Imm,
                  inAdd_R_RM_8,
                  inAdd_R_RM,
                  inAdd_RM_R_8,
                  inAdd_RM_R,
                  inAdc_RM_Imm_8,
                  inAdc_RM_ImmF,
                  inAdc_RM_Imm,
                  inAdc_R_RM_8,
                  inAdc_R_RM,
                  inAdc_RM_R_8,
                  inAdc_RM_R,
                  inSub_RM_Imm_8,
                  inSub_RM_ImmF,
                  inSub_RM_Imm,
                  inSub_R_RM_8,
                  inSub_R_RM,
                  inSub_RM_R_8,
                  inSub_RM_R,
                  inSbb_RM_Imm_8,
                  inSbb_RM_ImmF,
                  inSbb_RM_Imm,
                  inSbb_R_RM_8,
                  inSbb_R_RM,
                  inSbb_RM_R_8,
                  inSbb_RM_R,
                  inIMul_R_RM_Imm_8,
                  inIMul_R_RM_Imm,
                  inIMul_R_RM_8,
                  inIMul_R_RM,
                  inIDiv_RM_8,
                  inIDiv_RM,
                  inInc_RM_8,
                  inInc_RM,
                  inCmp_RM_R_8,
                  inCmp_RM_R,
                  inCmp_R_RM_8,
                  inCmp_R_RM,
                  inCmp_RM_Imm_8,
                  inCmp_RM_ImmF,
                  inCmp_RM_Imm,
                  inJmp_Rel,
                  inJl_Rel,
                  inJge_Rel,
                  inJe_Rel,
                  inJne_Rel,
                  inJle_Rel,
                  inJg_Rel;

public:
    InstructionManager_t () :
        emitter_          (),
        inMov_RM_Imm_8    ({0xC6}),
        inMov_RM_Imm      ({0xC7}),
        inMov_RM_R_8      ({0x88}),
        inMov_RM_R        ({0x89}),
        inMov_R_RM_8      ({0x8A}),
        inMov_R_RM        ({0x8B}),
        inPush_Imm_8      ({0x6A}),
        inPush_Imm        ({0x68}),
        inPush_RM         ({0xFF}),
        inPop_RM          ({0x8F}),
        inCall_RM         ({0xFF}),
        inCall_Rel        ({0xE8}),
        inRetn            ({0xC3}),
        inAdd_RM_Imm_8    ({0x80}),
        inAdd_RM_ImmF     ({0x83}),
        inAdd_RM_Imm      ({0x81}),
        inAdd_R_RM_8      ({0x02}),
        inAdd_R_RM        ({0x03}),
        inAdd_RM_R_8      ({0x00}),
        inAdd_RM_R        ({0x01}),
        inAdc_RM_Imm_8    ({0x80}),
        inAdc_RM_ImmF     ({0x83}),
        inAdc_RM_Imm      ({0x81}),
        inAdc_R_RM_8      ({0x12}),
        inAdc_R_RM        ({0x13}),
        inAdc_RM_R_8      ({0x10}),
        inAdc_RM_R        ({0x11}),
        inSub_RM_Imm_8    ({0x80}),
        inSub_RM_ImmF     ({0x83}),
        inSub_RM_Imm      ({0x81}),
        inSub_R_RM_8      ({0x2A}),
        inSub_R_RM        ({0x2B}),
        inSub_RM_R_8      ({0x28}),
        inSub_RM_R        ({0x29}),
        inSbb_RM_Imm_8    ({0x80}),
        inSbb_RM_ImmF     ({0x83}),
        inSbb_RM_Imm      ({0x81}),
        inSbb_R_RM_8      ({0x1A}),
        inSbb_R_RM        ({0x1B}),
        inSbb_RM_R_8      ({0x18}),
        inSbb_RM_R        ({0x19}),
        inIMul_R_RM_Imm_8 ({0x6B}),
        inIMul_R_RM_Imm   ({0x69}),
        inIMul_R_RM       ({0x0F, 0xAF}),
        inIDiv_RM_8       ({0xF6}),
        inIDiv_RM         ({0xF7}),
        inInc_RM_8        ({0xFE}),
        inInc_RM          ({0xFF}),
        inCmp_RM_R_8      ({0x38}),
        inCmp_RM_R        ({0x39}),
        inCmp_R_RM_8      ({0x3A}),
        inCmp_R_RM        ({0x3B}),
        inCmp_RM_Imm_8    ({0x80}),
        inCmp_RM_ImmF     ({0x83}),
        inCmp_RM_Imm      ({0x81}),
        inJmp_Rel         ({0xE9}),
        inJl_Rel          ({0x0F, 0x8C}),
        inJge_Rel         ({0x0F, 0x8D}),
        inJe_Rel          ({0x0F, 0x84}),
        inJne_Rel         ({0x0F, 0x85}),
        inJle_Rel         ({0x0F, 0x8E}),
        inJg_Rel          ({0x0F, 0x8F})
    {}

#define CHECK_SIZE_16(T) if (sizeof (T) == sizeof (short)) Emit66hPrefix ();
#define CHECK_SIZE_16_NO8(T) if (sizeof (T) == sizeof (short)) Emit66hPrefix ();

/*
#define CHECK_SIZE_16(T) if (sizeof (T) == sizeof (char)) ErrorPrintfBox ("char in %s", __PRETTY_FUNCTION__);
#define CHECK_SIZE_16_NO8(T) if (sizeof (T) == sizeof (char)) ErrorPrintfBox ("char in no8 %s", __PRETTY_FUNCTION__);
*/
#define SIZED_CMD(name) sizeof (T) != sizeof (char) ? name : name##_8

    template <typename T = int>
    void EmitMov (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_RM_R),
                                  regDest,
                                  regSrc);
    }

    template <typename T = int>
    void EmitMov (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_R_RM),
                                  MODE_ADDRESS,
                                  regDest,
                                  *regSrc);
    }

    template <typename T = int>
    void EmitMov (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_RM_R),
                                  MODE_ADDRESS,
                                  *regDest,
                                  regSrc);
    }

    template <typename T>
    void EmitMov (T* ptr, T imm)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_RM_Imm),
                                  MODE_ADDRESS,
                                  OFF, 0);
        emitter_.EmitData ((int32_t) ptr);
        emitter_.EmitData (imm);
    }

    template <typename T>
    void EmitMov (CPURegisterInfo_t regDest, T imm)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_RM_Imm), MODE_REGISTER, regDest, 0);
        emitter_.EmitData (imm);
    }

    template <typename T>
    void EmitMov (T* pointer, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_RM_R), MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData ((int32_t) pointer);
    }

    template <typename T>
    void EmitMov (CPURegisterInfo_t regDest, T* pointer)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inMov_R_RM), MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) pointer);
    }

    template <typename T>
    void EmitPush (T imm)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inPush_Imm), imm);
    }

    template <typename T>
    void EmitPush (T* mem)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inPush_RM, MODE_ADDRESS, OFF, 6);
        emitter_.EmitData ((int32_t) mem);
    }

    template <typename T = int>
    void EmitPush (CPURegisterInfo_t reg)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inPush_RM, reg, 6);
    }


    template <typename T = int>
    void EmitPop (CPURegisterInfo_t reg)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inPop_RM, reg, 0);
    }

    template <typename T = int>
    void EmitCall (CPURegisterInfo_t reg)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inCall_RM, reg, 2);
    }

    void EmitCallr (int32_t rel)
    {
        emitter_.EmitInstruction (inCall_Rel, rel);
    }

    template <typename T>
    void EmitCall (T* ptr)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inCall_RM, MODE_ADDRESS, OFF, 2);
        emitter_.EmitData ((int32_t) ptr);
    }

    void EmitRetn ()
    {
        emitter_.EmitInstruction (inRetn);
    }

    void SaveData ()
    {
        FILE* f = fopen ("Jit/jittest.bin", "wb");
        fwrite (emitter_.mcode_.buffer_.data (), 1, emitter_.mcode_.buffer_.size (), f);
        fclose (f);
    }

    template <typename T>
    void EmitAdd (CPURegisterInfo_t regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdd_RM_Imm), regDest, 0);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitAdd (CPURegisterInfo_t* regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdd_RM_Imm), MODE_ADDRESS, *regDest, 0);
        emitter_.EmitData (data);
    }

    template <typename T = int>
    void EmitAddF (CPURegisterInfo_t regDest, int8_t data)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inAdd_RM_ImmF, regDest, 0);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitAdd (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdd_R_RM), MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) src);
    }

    template <typename T>
    void EmitAdd (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdd_R_RM), MODE_ADDRESS, *regSrc, regDest);
    }

    template <typename T>
    void EmitAdd (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdd_RM_R), MODE_ADDRESS, *regDest, regSrc);
    }

    template <typename T>
    void EmitAdd (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdd_RM_R), MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData ((int32_t) dest);
    }

    template <typename T>
    void EmitAdc (CPURegisterInfo_t regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdc_RM_Imm), regDest, 2);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitAdc (CPURegisterInfo_t* regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdc_RM_Imm), MODE_ADDRESS, *regDest, 0);
        emitter_.EmitData (data);
    }

    template <typename T = int>
    void EmitAdcF (CPURegisterInfo_t regDest, int8_t data)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inAdc_RM_ImmF, regDest, 2);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitAdc (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdc_R_RM), MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) src);
    }

    template <typename T>
    void EmitAdc (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdc_R_RM), MODE_ADDRESS, *regSrc, regDest);
    }

    template <typename T>
    void EmitAdc (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdc_RM_R), MODE_ADDRESS, *regDest, regSrc);
    }

    template <typename T>
    void EmitAdc (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inAdc_RM_R), MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData ((int32_t) dest);
    }

    template <typename T>
    void EmitSub (CPURegisterInfo_t regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSub_RM_Imm), regDest, 5);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitSub (CPURegisterInfo_t* regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSub_RM_Imm), MODE_ADDRESS, *regDest, 0);
        emitter_.EmitData (data);
    }

    template <typename T = int>
    void EmitSubF (CPURegisterInfo_t regDest, int8_t data)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inSub_RM_ImmF, regDest, 5);
        emitter_.EmitData ((int32_t) data);
    }

    template <typename T>
    void EmitSub (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSub_R_RM), MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) src);
    }

    template <typename T>
    void EmiSub (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSub_R_RM), MODE_ADDRESS, *regSrc, regDest);
    }

    template <typename T>
    void EmitSub (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSub_RM_R), MODE_ADDRESS, *regDest, regSrc);
    }

    template <typename T>
    void EmitSub (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSub_RM_R), MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData ((int32_t) dest);
    }

    template <typename T>
    void EmitSbb (CPURegisterInfo_t regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSbb_RM_Imm), regDest, 3);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitSbb (CPURegisterInfo_t* regDest, T data)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSbb_RM_Imm), MODE_ADDRESS, *regDest, 0);
        emitter_.EmitData (data);
    }

    template <typename T = int>
    void EmitSbbF (CPURegisterInfo_t regDest, int8_t data)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inSbb_RM_ImmF, regDest, 3);
        emitter_.EmitData (data);
    }

    template <typename T>
    void EmitSbb (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSbb_R_RM), MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) src);
    }

    template <typename T>
    void EmitSbb (CPURegisterInfo_t regDest, CPURegisterInfo_t* regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSbb_R_RM), MODE_ADDRESS, *regSrc, regDest);
    }

    template <typename T>
    void EmitSbb (CPURegisterInfo_t* regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSbb_RM_R), MODE_ADDRESS, *regDest, regSrc);
    }

    template <typename T>
    void EmitSbb (T* dest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inSbb_RM_R), MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData ((int32_t) dest);
    }

    template <typename T, typename T_reg>
    void EmitMul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc, T data)
    {
        CHECK_SIZE_16 (T_reg)
        emitter_.EmitInstruction (SIZED_CMD (inIMul_R_RM_Imm), regDest, regSrc);
        emitter_.EmitData ((int32_t) data);
    }

    template <typename T>
    void EmitMul (CPURegisterInfo_t regDest, T* src)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inIMul_R_RM, MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) src);
    }

    template <typename T = int>
    void EmitMul (CPURegisterInfo_t regDest, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16_NO8 (T)
        emitter_.EmitInstruction (inIMul_R_RM, regDest, regSrc);
    }

    template <typename T = int>
    void EmitDiv (CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inIDiv_RM), regSrc, 7);
    }

    template <typename T>
    void EmitDiv (T* ptr)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inIDiv_RM), MODE_ADDRESS, OFF, 7);
        emitter_.EmitData ((int32_t) ptr);
    }

    template <typename T>
    void EmitInc (T* ptr)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inInc_RM), MODE_ADDRESS, OFF, 0);
        emitter_.EmitData ((int32_t) ptr);
    }

    template <typename T = int>
    void EmitInc (CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inInc_RM), MODE_REGISTER, regSrc, 0);
    }

    template <typename T>
    void EmitCmp (T* pointer, CPURegisterInfo_t regSrc)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inCmp_RM_R), MODE_ADDRESS, OFF, regSrc);
        emitter_.EmitData ((int32_t) pointer);
    }

    template <typename T>
    void EmitCmp (CPURegisterInfo_t regDest, T* pointer)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inCmp_R_RM), MODE_ADDRESS, OFF, regDest);
        emitter_.EmitData ((int32_t) pointer);
    }

    template <typename T>
    void EmitCmp (T* ptr, T imm)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (SIZED_CMD (inCmp_RM_Imm), MODE_ADDRESS, OFF, 7);
        emitter_.EmitData ((int32_t)ptr);
        emitter_.EmitData (imm);
    }

    template <typename T>
    void EmitCmpF (T* ptr, int8_t imm)
    {
        CHECK_SIZE_16 (T)
        emitter_.EmitInstruction (inCmp_RM_ImmF, MODE_ADDRESS, OFF, 7);
        emitter_.EmitData ((int32_t)ptr);
        emitter_.EmitData (imm);
    }


    void EmitJmp (int32_t rel)
    {
        emitter_.EmitInstruction (inJmp_Rel, rel);
    }

    void EmitJl (int32_t rel)
    {
        emitter_.EmitInstruction (inJl_Rel, rel);
    }

    void EmitJge (int32_t rel)
    {
        emitter_.EmitInstruction (inJge_Rel, rel);
    }

    void EmitJe (int32_t rel)
    {
        emitter_.EmitInstruction (inJe_Rel, rel);
    }

    void EmitJne (int32_t rel)
    {
        emitter_.EmitInstruction (inJne_Rel, rel);
    }

    void EmitJle (int32_t rel)
    {
        emitter_.EmitInstruction (inJle_Rel, rel);
    }

    void EmitJg (int32_t rel)
    {
        emitter_.EmitInstruction (inJg_Rel, rel);
    }

    void EmitRexW ()
    {
        emitter_.EmitData ((int8_t) 0x48);
    }

    void Emit66hPrefix ()
    {
        emitter_.EmitData ((int8_t) 0x66);
    }

    void EmitInt3 ()
    {
        emitter_.EmitData ((uint8_t) 0xCC);
    }

    void BuildAndRun ()
    {
        emitter_.mcode_.BuildAndRun ();
    }

    size_t Size ()
    {
        return emitter_.mcode_.Size ();
    }

    std::vector<uint8_t>* GetData ()
    {
        return &emitter_.mcode_.buffer_;
    }

#undef CHECK_SIZE_16
#undef CHECK_SIZE_16_NO8

};
#endif // INSTRUCTION_H_INCLUDED
