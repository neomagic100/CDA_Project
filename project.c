#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

/* instruction fetch */
/* 10 Points */
//Mem is array already populated
// PC is current index to use for instruction memory
// Check for word alignment
// Use PC >> 2 to get actual location (b/c Mem is an array of words in C)
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    unsigned memAtLocation = Mem[PC >> 2];

    //TODO Check for word alignment

    *instruction = memAtLocation;
}


/* instruction partition */
/* 10 Points */
// No need to check op code. Make copies of all sections.
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    const int OP_BITS = 6;
    const int FUNCT_BITS = 6;
    const int R_BITS = 5;
    const int OFFSET_BITS = 16;
    const int JSEC_BITS = 26;
    const int TOTAL_BITS = 32;

    // Isolate each set of bits
    *op = instruction >> (TOTAL_BITS - OP_BITS);
    *r1 = (instruction << OP_BITS) >> (TOTAL_BITS - R_BITS);
    *r2 = (instruction << (R_BITS + OP_BITS)) >> (TOTAL_BITS - R_BITS);
    *r3 = *r2 = (instruction << (R_BITS + R_BITS + OP_BITS)) >> (TOTAL_BITS - R_BITS);
    *funct = (instruction << (TOTAL_BITS - FUNCT_BITS)) >> (TOTAL_BITS - FUNCT_BITS);
    *offset = (instruction << (TOTAL_BITS - OFFSET_BITS)) >> (TOTAL_BITS - OFFSET_BITS);
    *jsec = (instruction << (TOTAL_BITS - JSEC_BITS)) >> (TOTAL_BITS - JSEC_BITS);
}



/* instruction decode */
/* 15 Points */
// Based on opcode, set control signals in controls structure.
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
// Set parameters for A, B, and ALUControl
// If R-Type look at funct
// In end, call ALU function
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
// PC = PC + 4
// Take care of Branch and Jump
// Zero - Branch taken or not
// jump: left shift bits of jsec and use upper 4 bits of PC
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}
