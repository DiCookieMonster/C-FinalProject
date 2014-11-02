/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================

 */

#include "instruction.h"
#include "symbol.h"
#include "dispose.h"

extern int countLine;


/* This array contain all legal instructions */
instruction constInstructionArr[MAX_INSTRUCTION+1]= {
			{"mov",0,{0,0},0,0,{1,1,1,1,1},{0,1,1,1,1},2},
			{"cmp",0,{0,0},0,1,{1,1,1,1,1},{1,1,1,1,1},2},
			{"add",0,{0,0},0,2,{1,1,1,1,1},{0,1,1,1,1},2},
			{"sub",0,{0,0},0,3,{1,1,1,1,1},{0,1,1,1,1},2},
			{"not",0,{0,0},0,4,{0,0,0,0,0},{0,1,1,1,1},1},
			{"clr",0,{0,0},0,5,{0,0,0,0,0},{0,1,1,1,1},1},
			{"lea",0,{0,0},0,6,{0,1,1,1,0},{0,1,1,1,1},2},
			{"inc",0,{0,0},0,7,{0,0,0,0,0},{0,1,1,1,1},1},
			{"dec",0,{0,0},0,8,{0,0,0,0,0},{0,1,1,1,1},1},
			{"jmp",0,{0,0},0,9,{0,0,0,0,0},{0,1,1,1,1},1},
			{"bne",0,{0,0},0,10,{0,0,0,0,0},{0,1,1,1,1},1},
			{"red",0,{0,0},0,11,{0,0,0,0,0},{0,1,1,1,1},1},
			{"prn",0,{0,0},0,12,{0,0,0,0,0},{1,1,1,1,1},1},
			{"jsr",0,{0,0},0,13,{0,0,0,0,0},{0,1,0,0,0},1},
			{"rts",0,{0,0},0,14,{0,0,0,0,0},{0,0,0,0,0},0},
			{"stop",0,{0,0},0,15,{0,0,0,0,0},{0,0,0,0,0},0},
			{".entry",0,{0,0},0,16,{0,0,0,0,0},{0,1,0,0,0},1},
			{"null",0,{0,0},0,17,{0,0,0,0,0},{0,0,0,0,0},0}
};

/* This array contains all registers */
reg strRegister[MAX_REGISTER]= {
		{"r0",0},
		{"r1",1},
		{"r2",2},
		{"r3",3},
		{"r4",4},
		{"r5",5},
		{"r6",6},
		{"r7",7}
};



/* This function counts spaces */
int SpaceCount (int model)
{
	if (model==IMMEDIATE || model==LABEL)
		return 1;
	else if (model==LABEL_PLUS_STAR_LABEL)
		return 2 ;
	else if (model==REGISTER)
		return 0;
	else if (model==LABEL_PLUS_LABEL_PLUS_REGISTER)
		return 3;
	return 0;
}

/* If the line included the label, this function checks if this label already defined in .string guidance */
int CheckLabel (char *line)
{
	int i=0;
	/* IgnoreSpaces */
	while(isspace(line[i]))
		i++;

	/* check if is not .string guidance */
	if(line[i]=='.')
		return 0 ;
	else
	{
		for ( i=0 ; i<MAX_LABEL ; i++)
		{
			if(line[i]==':')
			{
				/* returns length of a label */
					return i;
			}
		}
	}

	return 0 ;
}

/* This function checks that name label is not equals to name of register or instruction */
int CheckLabelNameLegality (char *label)
{
	int i;
	/* search in instruction */
	for ( i=0; i < MAX_INSTRUCTION ; i++ )
	{
		if (strcmp(label,constInstructionArr[i].name) == 0)
			return 0;
	}
	/* search in register */
	 if (RegisterLookup(label))
		return 0 ;

	return 1;
}

/* This function checks if the next word is Virtual instruction
 (like .string or .data) */
int CheckIfVirtualInstruction(char *line , int isLabel)
{
	int i;
	char *word;
	/* Set i position for case you have label and for case you don't have a label */
	i = isLabel ? isLabel+2 : 0;

	/* Ignore spaces */
	while(isspace(line[i]))
		i++;

	/* Check if */
	if(line[i++]=='.')
	{
		word=GetWord(&line[i],' ');
		if(strcmp(word,"data")==0)
		{
			if(isLabel>0)
			{
				SetLabel(line,(int)isLabel,DATA,0,0,(int)DC);
			}
			/* Step over .data word in line */
			i+=5;
			/* Ignore spaces */
			while(isspace(line[i]))
				i++;

			HandleData(&line[i]);
			return 1;

		}
		else if(strcmp(word,"string")==0)
		{
			if(isLabel>0)
			{
				SetLabel(line,(int)isLabel,DATA,0,0,(int)DC);
			}
			/* Step over .string word in line */
			i += 7;
			/* Step over space */
			while(isspace(line[i]))
				i++;

			HandleString(&line[i]);
			return 1;
		}
	}
	else
	{
		return 0;
	}
	return 0;
}



/* This function checks if the next word is Guidance instruction
 (like .extern or .entry) */

int CheckIfGuidance(char *line,int isLabel)
{
	int i;
	char *word;

	i = isLabel ? isLabel + 2 : 0;
	/* Ignore spaces */
	while(isspace(line[i]))
		i++;

	if (line[i++]=='.')
	{
		word=GetWord(&line[i],' ');
		if(strcmp(word,"extern")==0)
		{
			/* Step over .extern word in line */
			i+=7;
			/* Ignore spaces */
			while(isspace(line[i]))
				i++;

			HandleExtern(&line[i]);
			return 1;
		}
		else if(strcmp(word,"entry")==0)
		{
			i+=6;
			while(isspace(line[i]))
				i++;
			HandleEntry(&line[i],&constInstructionArr[MAX_INSTRUCTION-1]);
			return 1;
		}
		else
		{
			printf("Error! in Line %d  :%s  , illegal command \n", countLine, line);
			SeconnedPhaseSucess=0;
		}
	}
	else
	{
		return 0;
	}
	return 0;
}


/* This function is the last function to check if there is such command in assembly language */
void CheckIfSimpleInctruction (char *line , int isLabel)
{
	int i,L = 1;
	int countArgumentInLine = 0;
	int typeForAddressing = SOURCE;
	char *command;
	char *argument[MAX_LINE];
	int model[MAX_LINE];
	int registerNum[MAX_LINE];
	int len;
	instruction* ins;

	len=strlen(line);
	/* Check if there is a label */
	if(isLabel)
	{
		SetLabel(line ,isLabel + 1, CODE, 0, 0, IC);
		/* Sets new position . over ':' and set position in space or in instruction; */
		isLabel += 2;
	}

	/* Sets new position */
	i=isLabel;
	/* Ignore label */
	while(isspace(line[i]))
		i++;

	/* Get command */
	command = GetWord(&line[i],'/');
	i++;

	/* Send to InstructionLookup function and return instruction */
	ins = InstructionLookup(command);

	/* command not exist */
	if(strcmp(ins->name,"null")==0)
	{
		printf("Error! in line %d : %s   illegal command \n", countLine,line);
		FirstPhaseSuccess=0; 
		
	}
	else /*command exist*/
	{
		/* Set new position after the command */
		i+=strlen(command);
		if(checkType(&line[i], countLine, ins,command)==0)
		{ 
		  printf("Error! in line %d : %s   illegal command \n", countLine,line);
		}
		if(ins->type==0)
			i+=4;

		if(ins->type==1)
			i+=8;

		/* Check if we need to look for argument and method of addressing for the current command */
		if (ins->operandCount>0)
		{
			for (;i<len;)
			{
				int legalAddressing;
				/* Get a name of an argument */
				argument[countArgumentInLine]=GetWord(&line[i],',');
				if (strcmp(argument[countArgumentInLine],"") != 0)
				{
					/* Identify method of addressing */
					model[countArgumentInLine] = IdentifyOperandType(argument[countArgumentInLine]);
					/* Check model to identify how many spaces to turn */
					L += SpaceCount(model[countArgumentInLine]);

					registerNum[countArgumentInLine]=WhichRegister(argument[countArgumentInLine],model[countArgumentInLine]);
					/* For case i have single operand */
					if(ins->operandCount == 1)
						typeForAddressing++;
					/* Check legality of adressing method for this command */
					legalAddressing=CheckAdressingLegality(command,typeForAddressing++,model[countArgumentInLine++]);

					if(!(legalAddressing))
					{
						printf("Error! in Line %d:%s argument: %s is not legal addressing \n",countLine, line , argument[countArgumentInLine-1]);
						FirstPhaseSuccess=0;
					}
					/* Set new position */
					i+=strlen(argument[countArgumentInLine-1]);
					/* Check if you are in the range , and step over space and ',' if exist */
					if(i < len)
					{
						while((line[i] != ',') && (line[i] != '\0'))
							i++;
						while(isspace(line[i]))
							i++;
						if(line[i]==',')
							i++;
					}
				}
				else
				{
					printf("Error! in Line %d :%s argument is empty \n",countLine,line);
					FirstPhaseSuccess=0;
					break;
				}
			}
		}

		/* Check value count of arguments */
		if(countArgumentInLine > ins->operandCount)
		{
			printf("Error! in Line %d: %s to many arguments \n",countLine, line);
			FirstPhaseSuccess=0;
		}
		else if(countArgumentInLine< ins->operandCount)
		{
			printf("Error! in Line %d: %s not enogth arguments \n",countLine, line);
			FirstPhaseSuccess=0;
		}

		else
		{
			/* Build Inctruction struct */
			InsertInstructionToList(ins,&*argument,model,registerNum);
			/* Update IC */
			IC += L;
		}
	}
}

/* This function checks two adressing method.
1.LABEL_PLUS_LABEL_PLUS_REGISTER;
2.LABEL_PLUS_STAR_LABEL
*/
int CheckOperand (char *operand)
{
	char *pch;
	char *pch1;
	char *star;
	char register_[MAX_LINE] ;
	int indexBrackets;
	/* Look for '[' char */
	pch=strchr(operand,'{');
	star=strchr(operand,'*');

	if ((pch!=NULL) && (star!=NULL))
	{
		if ((star-pch)==1)
		{
			/* Save index to search inside */
			indexBrackets = pch-operand ;
			/* Search Bracket to '}' */
			pch=strchr(operand,'}');
			if(pch!=NULL)
			{
				/* Copy a word within Brackets */
				strncpy (register_,&operand[indexBrackets+2], pch-star-1);
				register_[pch-star-1]='\0';
				/* Check model */
				return LABEL_PLUS_STAR_LABEL;
			}
			else
			{
				printf("Error! in Line %d:  %s , illegal operand description \n",countLine, operand);
				FirstPhaseSuccess=0;
			}
		}
	}

	else if (pch !=NULL)
	{
		pch1 = strchr(pch,'{');
		/* Save index to search inside */
		indexBrackets = pch1-operand ;
		/* Search Bracket to '[' */
		pch = strchr(pch1,'}');
		if(pch ==NULL)
		{
			/* Copy a word within Brackets */
			strncpy(register_,&operand[indexBrackets+1],2);
			register_[pch-operand-indexBrackets-1]='\0';
			/* Check model */
			if(pch !=NULL)
			{
				/* Save index to search inside */
				indexBrackets = pch-operand ;
				/* Search Bracket to '[' */

				if(RegisterLookup(register_))
					{FirstPhaseSuccess=1; return LABEL_PLUS_LABEL_PLUS_REGISTER;}
				else
				{
					printf("Error! in Line %d:  %s - Unknown Register  \n",countLine, operand);
	  	  			FirstPhaseSuccess=0;
				}
			}
		}
	   }

	FirstPhaseSuccess=1;
	return LABEL;
}


/* This function search in instruction struct and return TRUE(1) or FALSE(0) */
int RegisterLookup (char *register_)
{
	int i;
	for (i = 0; i < MAX_REGISTER ;i++)
	if (strcmp(register_,strRegister[i].name) == 0)
		return 1;
	return 0;
}

/* Type is destination or source
position - in arr
this function check if addressing method is legal */
int CheckAdressingLegality (char *command , int type , int position)
{
	int i;
	for (i = 0; i < MAX_INSTRUCTION; i++)
	if (strcmp(command,constInstructionArr[i].name) == 0)
	{
		if(type==SOURCE)
		{
			return constInstructionArr[i].sourceUse[position] ? 1 : 0;
		}
		else if (type == DEST)
		{
			return constInstructionArr[i].destUse[position] ? 1 : 0;
		}
	}
	return 0;
}



