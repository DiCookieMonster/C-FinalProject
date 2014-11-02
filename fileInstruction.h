/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 #include "global.h"

enum operandTypes { IMMEDIATE,LABEL,LABEL_PLUS_STAR_LABEL,LABEL_PLUS_LABEL_PLUS_REGISTER,REGISTER};

/* Struct that represents an instruction */
/* This struct represents a processor's ISA (not same to fileInstruction)*/
typedef struct {
    char name[5];
    int type;
    int TypeVal[2];
    int DBL;
    int code;
    int sourceUse[5];
    int destUse[5];
    int operandCount;
} instruction;

/* This struct represents an operand */
typedef struct
{
	char name[MAX_LINE];
	int reg;
	int operandType;
}operand;

/* This struct represents a node for file instruction array, represents an instruction read from the file */
typedef struct
{
    instruction* ins;
    operand op1 ;
    operand op2;
} fileInstruction;

void AddFileInstruction (instruction* ins,char op1Name[MAX_LINE],int op1Type,int registerNum1,char op2Name[MAX_LINE], int op2Type,int registerNum2);
void InsertInstructionToList (instruction* ins,char *argument[],int model[],int registerNum[]);
