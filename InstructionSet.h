#ifndef INSTRUCTIONSET_H_INCLUDED
#define INSTRUCTIONSET_H_INCLUDED

enum Instructions
{
    ASM_PUSH_IMM = 1,
    ASM_POP,
    ASM_MOV_TO_EAX,
    ASM_CALL,
    ASM_LEA,
    ASM_ADD,
    ASM_SUB,
    ASM_MUL,
    ASM_DIV
};

class CmdEmitterFiller_t
{
    CmdEmitter_t& emitter_;
    public:
    CmdEmitterFiller_t (CmdEmitter_t* emitter) :
        emitter_ (*emitter)
    {

    }

};

#endif // INSTRUCTIONSET_H_INCLUDED
