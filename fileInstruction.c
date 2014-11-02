/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileInstruction.h"

int GetIndexByInstruction (instruction *inc);

int FIC=0;
fileInstruction fInsArr[MAX_ARRAY_LENGTH];

/* This function adds instruction to file instruction array */
void AddFileInstruction (instruction* ins,char op1Name[MAX_LINE],int op1Type,int registerNum1,char op2Name[MAX_LINE], int op2Type,int registerNum2)
{
	fInsArr[FIC].ins = ins;
        strcpy(fInsArr[FIC].op1.name,op1Name);
        fInsArr[FIC].op1.operandType = op1Type;
	fInsArr[FIC].op1.reg = registerNum1 == -1 ? 0 : registerNum1;
        strcpy(fInsArr[FIC].op2.name,op2Name);
	fInsArr[FIC].op2.operandType = op2Type;
	fInsArr[FIC].op2.reg= registerNum2 == -1 ? 0 : registerNum2;


    FIC++;
}


/* This fuction gets pointer to instruction , argument and addressing method */
void InsertInstructionToList (instruction* ins,char *argument[],int model[],int registerNum[])
{
	if (ins->operandCount==0)
		AddFileInstruction (ins,"",0,-1,"" ,0,-1);
	else if (ins->operandCount==1)
		AddFileInstruction (ins,"{" ,0,-1,argument[0],model[0],registerNum[0]);
	else if (ins->operandCount==2)
		AddFileInstruction (ins,argument[0],model[0],registerNum[0],argument[1] ,model[1],registerNum[1]);

}
