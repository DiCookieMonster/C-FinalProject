/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================

 */
 
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"

/* This struct represents an instruction */
typedef struct {
    char name[6];
    int type;
    int TypeVal[2];
    int DBL;
    int code;
    int sourceUse[5];
    int destUse[5];
    int operandCount;
} instruction;

/* This struct represents a register */
typedef struct {
    char *name;
    int code;
} reg;

extern int DC;
extern int IC;
extern int DataArray[MAX_ARRAY_LENGTH];


int WhichRegister (char *_register,int model);
int SpaceCount (int model);
char *GetWord (char *line , char stop);
int IsDigitCustom (char* num);
void HandleString(char* string);
void SetLabel (char *line,int len,int type,int isExtern, int isEntery , int address);
void HandleData (char* data);
void HandleEntry (char* label, instruction* ins);
void HandleExtern (char* label);
int AddSymbol (char* name, int type, int isExtern, int isEntry, int address);
int IdentifyOperandType (char *operand);
instruction* InstructionLookup (char *Instruction);
int RegisterLookup (char *register_);
int CheckAdressingLegality (char *command , int type , int position);
int CheckOperand (char *operand);
void InsertInstructionToList (instruction* ins,char *argument[],int model[],int registerNum[]);
void AddFileInstruction (instruction* ins,char op1Name[MAX_LINE],int op1Type,int registerNum1,char op2Name[MAX_LINE], int op2Type,int registerNum2);
int checkType(char* line, int countLine, instruction* ins,char *command);
 
enum guidanceTypes { DATA_GUIDANCE = 1, STRING_GUIDANCE };
enum operandTypes { IMMEDIATE,LABEL,LABEL_PLUS_STAR_LABEL,LABEL_PLUS_LABEL_PLUS_REGISTER,REGISTER};
enum typeUse {SOURCE,DEST};
