#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    signed Asigned = (signed)A;
    signed Bsigned = (signed)B;

    switch (ALUControl) {
        case 0: // Add
            *ALUresult = A + B;
            break;
        case 1: // Subtract
            *ALUresult = A - B;
            break;

        case 2: // slt
            *ALUresult = ((Asigned - Bsigned) < 0);
            break;
        case 3: // sltu                      // FIXME
            *ALUresult = ((B - A) > 0);
            break;


        case 4: // And
            *ALUresult = A & B;
            break;
        case 5: // Or
            *ALUresult = A | B;
            break;
        case 6: // shift left B by 16 bits
            *ALUresult = B << 16;
            break;
        case 7: // not A
            *ALUresult = ~A;
            break;
        default:
            break;
    }

    if (*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
//Mem is array already populated
// PC is current index to use for instruction memory
// Check for word alignment
// Use PC >> 2 to get actual location (b/c Mem is an array of words in C)
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    unsigned address = PC >> 2;

    if (PC % 4 != 0)
      return 1;

    //TODO Check for word alignment

    *instruction = Mem[address];

    return 0;
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
    *r3 = (instruction << (R_BITS + R_BITS + OP_BITS)) >> (TOTAL_BITS - R_BITS);
    *funct = (instruction << (TOTAL_BITS - FUNCT_BITS)) >> (TOTAL_BITS - FUNCT_BITS);
    *offset = (instruction << (TOTAL_BITS - OFFSET_BITS)) >> (TOTAL_BITS - OFFSET_BITS);
    *jsec = (instruction << (TOTAL_BITS - JSEC_BITS)) >> (TOTAL_BITS - JSEC_BITS);
}



/* instruction decode */
/* 15 Points */
// Based on opcode, set control signals in controls structure.
int instruction_decode(unsigned op,struct_controls *controls)
{   // Initially set all controls to 0
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;

    if (op == 0x0) { // R-Type
        controls->RegDst = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 7;
    }
    else if (op == 0x2 || op == 0x3) { // J-Type
        controls->Jump = 1;
        controls->RegDst = 2;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->ALUSrc = 2;
    }
    else if (op == 0x4 || op == 0x5) {
        controls->ALUOp = 1;
        controls->Branch = 1;
        controls->RegDst = 2;
        controls->MemtoReg = 2;
    }
    else if (op >= 0x8 && op <= 0xF) { // I-Type Arithmetic/Logic
        controls->ALUSrc = 1;
        controls->RegWrite = 1;

        if (op == 0xA) //slt
            controls->ALUOp = 2;
        if (op == 0xB) //sltu
            controls->ALUOp = 3;
        if (op == 0xC) //andi
            controls->ALUOp = 4;
        if (op == 0xD) //ori
            controls->ALUOp = 5;
        if (op == 0xF) //lui
            controls->ALUOp = 6;
    }       
    else if (op >= 0x20 && op <= 0x23) { // load
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    else if (op >= 0x28 && op <=0x2B) { // store
        controls->MemtoReg = 2;
        controls->RegDst = 2;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
    }
    else {
        return 1;
    }

    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    // Make sure first 16 bits are 0
    if ((offset >> 16) != 0) {
        *extended_value = offset;
        return;
    }

    // Check if 16th bit is 1 (negative) or 0 (positive)
    // If positive, no need to sign extend
    if ((offset >> 15) == 1) {
        *extended_value = (offset | 0xFFFF0000);
    }
    else
    	*extended_value = offset;
}

/* ALU operations */
/* 10 Points */
// Set parameters for A, B, and ALUControl
// If R-Type look at funct
// In end, call ALU function
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    char ALUControl;
    unsigned A = data1;
    unsigned B = (ALUSrc == 0) ? data2 : extended_value;

    if (ALUSrc == 0) { // R-type - look at funct
        switch (funct) {
            case 0x0: //sll
                ALUControl = 6;
                break;
            case 0x21: //addu //FIXME
            case 0x20: //add
                ALUControl = 0;
                break;
            case 0x22: //subtact
            case 0x23: //subu // FIXME
                ALUControl = 1;
                break;
            case 0x24: //and
                ALUControl = 4;
                break;
            case 0x25: //or
                ALUControl = 5;
                break;
            case 0x27: //not
                ALUControl = 7;
                break;
            case 0x2A: // slt
                ALUControl = 2;
                break;
            case 0x2B: //sltu //FIXME
                ALUControl = 3;
                break;
            default:
                return 1;
        }
    }
  
    else {
        ALUControl = ALUOp; //Not sure of this line. (Placeholder)
    }

    ALU(A, B, ALUControl, ALUresult, Zero);

    // Return 1 if halt occurs, otherwise return 0
    return 0;

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // Check memory alignment


    if (MemWrite == 1) {
        Mem[ALUresult>>2] = data2;
    }

    if (MemRead == 1) {
    	if (ALUresult % 4 != 0)
    	        return 1;
        *memdata = Mem[ALUresult>>2];
    }

    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    unsigned writeReg = (RegDst == 0) ? r2 : r3;
    //writeReg *= 4;

    // data coming from memory
    if (RegWrite == 1 && MemtoReg == 1) {
        Reg[writeReg] = memdata;
    }

    // data coming from ALU ALUresult
    if (RegWrite == 1 && MemtoReg == 0) {
        Reg[writeReg] = ALUresult;
    }
}

/* PC update */
/* 10 Points */
// PC = PC + 4
// Take care of Branch and Jump
// Zero - Branch taken or not
// jump: left shift bits of jsec and use upper 4 bits of PC
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    if (Jump == 0) {
        *PC += 4;

        // Branch taken
        if (Branch == 1 && Zero == 1) {
            *PC += 4 * extended_value;
        }


    }

    else {
        unsigned tempPC = *PC;

        // use upper 4 bits of PC
        tempPC >>= 28;
        tempPC <<= 28;

        //shift jsec left 2 and use upper bits of PC
        jsec <<= 2;

        *PC = tempPC | jsec;
    }
}
