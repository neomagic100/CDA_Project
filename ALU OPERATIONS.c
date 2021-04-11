
/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    // if the ALUSrc is 1, set data2 to extended_value
	if (ALUSrc == 1) {
		data2 = extended_value;
	}

	// 7 is R-type instruction
    if (ALUOp == 7) {
    	// set the correct ALUOp for each R-type instruction
    	switch(funct) {
	    	// add
	    	case 32:
	    		ALUOp = 0;
	    		break;
	    	// subtract
	    	case 34:
	    		ALUOp = 1;
	    		break;
	    	// set less than signed
	    	case 42:
	    		ALUOp = 2;
	    		break;
	    	// set less than unsigned
	    	case 43:
	    		ALUOp = 3;
	    		break;
	    	// and
	    	case 36:
	    		ALUOp = 4;
	    		break;
	    	// or
	    	case 37:
	    		ALUOp = 5;
	    		break;
	    	// shift left right variable
	    	case 4:
	    		ALUOp = 6;
	    		break;
	    	// nor
	    	case 39:
	    		ALUOp = 7;
	    		break;
	    	// default for halt or don't care
	    	default:
	    		return 1;
	    }
	    // call ALU function
	    ALU(data1, data2, ALUOp, ALUresult, Zero);
    } else {
    	// call ALU for non-functions
    	ALU(data1, data2, ALUOp, ALUresult, Zero);
    }

    return 0;
}

