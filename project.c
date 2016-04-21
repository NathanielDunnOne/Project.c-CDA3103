
#include "spimcore.h"

#define TRUE 1
#define FALSE 0

// Helper function for convertToSigned
long twoPower(long pow){

    long result = 1;
    int i;

    for(i = 0; i < pow; i++){
        result = 2 * result;
    }

    return result;
}

// Convert an unsigned number to a signed number
long convertToSigned(unsigned a){

    unsigned copy = a;
    int array[32];
    int i;
    long j;
    long maxU = -2147483647;

    // Create a binary representation of a in an array
    for(i = 31; i >= 0; i--){
        copy = copy/2;
        if(copy%2 != 0){
            array[i] = 1;
        }
        else
            array[i] = 0;
    }

    // If the first position in the array is a 0, then the number is
    // positive, and a doesn't change
    if(array[0] == 0){
        return a;
    }

    // Otherwise, convert it to unsigned by taking the minimum possible signed
    // number and adding 2^j for every position of 1
    for(i = 1, j = 30; i < 32; i++, j--){
        if(array[i] == 1){
            maxU = maxU + twoPower(j);
        }
    }

    // Return the unsigned number
    return maxU - 1;
}

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    if(ALUControl == 0){
        A = convertToSigned(A);
        B = convertToSigned(B);
        *ALUresult = A + B;
    }
    if(ALUControl == 1){
        A = convertToSigned(A);
        B = convertToSigned(B);
        *ALUresult = A - B;
    }
    if(ALUControl == 2){
        A = convertToSigned(A);
        B = convertToSigned(B);
        if(A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }
    if(ALUControl == 3){
        if(A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }
    if(ALUControl == 4){
        *ALUresult = (A & B);
    }
    if(ALUControl == 5){
        *ALUresult = (A | B);
    }
    if(ALUControl == 6){
        *ALUresult = (B << 16);
    }
    if(ALUControl == 7){
        *ALUresult = (~A);
    }
    if(*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    //Send the PC counter and fetch the intruction from that memory
    if(PC%4 == 0){
        *instruction = Mem[ (PC >> 2) ];
        return 0;
    }
    else
        return 1;

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

    unsigned tmp;

    // instruction [31-26]
    tmp = instruction >> 26;
    tmp = tmp & 0b00000000000000000000000000111111;
    *op = tmp;

    // instruction [25-21]
    tmp = instruction >> 21;
    tmp = tmp & 0b00000000000000000000000000011111;
	*r1 = tmp;

    // instruction [20-16]
    tmp = instruction >> 16;
    tmp = tmp & 0b00000000000000000000000000011111;
    *r2	= tmp;

    // instruction [15-11]
    tmp = instruction >> 11;
    tmp = tmp & 0b00000000000000000000000000011111;
    *r3	= tmp;

    // instruction [5-0]
    tmp = instruction;
    tmp = tmp & 0b00000000000000000000000000111111;
    *funct = tmp;

    // instruction [15-0]
    tmp = instruction & 0b00000000000000001111111111111111;
    *offset = tmp;

    // instruction [25-0]
    *jsec   = instruction & 0b00000011111111111111111111111111;

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // R type
    if(op == 0){
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        return 0;
    }

    // Add immediate
    if(op == 8){
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }

    // Load word
    if(op == 35){
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }

    // Store word
    if(op == 43){
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
        return 0;
    }

    // Load upper immediate
    if(op == 15){
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 6;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }

    // Branch on equal
    if(op == 4){
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 1;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
        return 0;
    }

    // Set less than immediate
    if(op == 10){
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 2;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }

    // Set less than immediate unsigned
    if(op == 11){
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 3;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }

    // Jump
    if(op == 2){
        controls->RegDst = 1;
        controls->Jump = 1;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
        return 0;
    }

    // Invalid Opcode
    return 1;

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // Read registers at address r1 and r2 and write them into data1 and data2
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    // If the 16th bit is 0, the offset value is positive, so
    // it doesn't have to be changed
    if(offset < twoPower(15)){
        *extended_value = offset;
    }
    // Otherwise, the 16th bit is 1, and the offset value is a negative,
    // so fill bits 17-32 with 1s
    else{
        *extended_value = offset | 0b11111111111111110000000000000000;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

    if(ALUSrc == 1)
    {
      data2 = extended_value;
    }

    if(ALUOp == 7) {

        switch(funct) {

        case 32: //Add
        ALUOp = 0;
        break;

        case 34: //Subtract
        ALUOp = 1;
        break;

        case 36: //AND
        ALUOp = 4;
        break;

        case 37: //OR
        ALUOp = 5;
        break;

        case 42: //Signed less than
        ALUOp = 2;
        break;

        case 43: //Unsigned less than
        ALUOp = 3;

        case 6: //Shift 16 bits left
        ALUOp = 6;
        break;

        case 7: //Not
        ALUOp = 7;
        break;

            ALU(data1, extended_value, ALUOp, ALUresult, Zero);

            }
    }

    else {

        ALU(data1, extended_value, ALUOp, ALUresult, Zero);

    }

    //Halt if there is an illegal instruction
    if(ALUOp > 7 ||  funct > 43 || ALUSrc > 1)
        return 1;

    return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //We initialize MemWrite to be True so we are storing the data
    if(MemWrite == TRUE) {
        //If we were to swap the values we would be loading the data not storing it.
        Mem[ALUresult >> 2] = *memdata;
        //halting the event
        if(ALUresult % 4 != 0) {

            return 1;

        }

    }
    //Since MemRead to True we are now loading the data
    if(MemRead == TRUE) {
        //If the data2 was after Men then we would be storing the value, but right now we are loading
        data2 = Mem[ALUresult >> 2];
        //Halting the event
        if(ALUresult % 4 != 0) {

            return 1;

        }

    }

    return 0;

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite == TRUE) {
        //Only if the memory to the register
        if(MemtoReg == TRUE) {
	    //Register destination of the instruction
            if(RegDst == TRUE) {
		//instructions from [15-11]
                Reg[r3] = memdata;

            }
	    //Register destination of the instruction
            if(RegDst == FALSE) {
		//instruction from [20-16]
                Reg[r2] = memdata;

            }

        }
	//If MemtoReg is false then it skips reading in the data.
        //It goes from ALUresult to the MemtoReg multiplexer.
        if(MemtoReg == FALSE) {
	    //Register destination of the instruction.
            if(RegDst == TRUE) {

                Reg[r3] = ALUresult;

            }
	    //Register destination of the instruction.
            if(RegDst == FALSE) {

                Reg[r2] = ALUresult;
            }
        }

    }

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //If Jump is asserted we take the jump address (26 bits), shift it left 2 (28 bits),
    //then concatinate the upper 4 bits of PC + 4 to the higher-order bits (32 bits)
    if(Jump == 1)
    {
        jsec = jsec << 2;
        *PC += 4;
        *PC = *PC >> 28; //
        *PC = *PC << 28; //this makes the 28 lower-order bits 0
        *PC = *PC + jsec;
    }

    //If Branch and Zero are asserted we take the extended_value, shift it left 2,
    //then add it to the current PC + 4
    else if(Branch == 1 && Zero == 1)
    {
        extended_value = extended_value << 2;
        *PC += 4;
        *PC += extended_value;
    }

    //If both Jump and Branch are deasserted, just increment the PC by 4
    else
        *PC += 4;
}
