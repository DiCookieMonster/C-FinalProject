/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 
#include "instruction.h"
#include "symbol.h"
#include "dispose.h"

extern instruction constInstructionArr[MAX_INSTRUCTION];
extern reg strRegister[MAX_REGISTER];
extern int countLine;

/* This function handle in VirtualInstruction : .data */
void HandleData (char* data)
{
	char num[MAX_LINE];
	int i=0;
	int len;
	num[0] = 0;
	len = strlen(data);
	/* Get the first number */
	strcpy(num,GetWord(&data[i],','));
	/* Check if the number exist */
	if (num[0])
	{
		/* Run all over line */
		for(;i<len-1;)
		{
			/* Set i for a new position */
			i+=strlen(num)+1;
			/* Check if the num is really numeric */
			if(IsDigitCustom(num))
			{
				/* Set num in the arr */
				DataArray[DC++] = atoi (num);
			}
			else
			{
				printf("Error! Line number : %d data : \" %s \" the value : %s , is not a numeric!\n",countLine,data ,num);
				FirstPhaseSuccess=0;
			}
			/* Copy next value */
			strcpy(num,GetWord(&data[i],','));
		}
	}
	else
	{
		printf("Error! Line number : %d in data : %s , no value \n",countLine, data);
		FirstPhaseSuccess=0;
	}

}

/* This function handle in VirtualInstruction : .string */
void HandleString(char* string)
{
	int j,i=0;
	int LastPosition = 0;
	int len;
	
	len = strlen(string);
	if(len>0)
	{
		/* Check if first char is " */
		if(string[i++] =='"')
		{
			/* Check last sign '"' */
			j=i;
			while (j < len)
			{
				if(string[j++]=='"')
					LastPosition=j-1;

			}
			if(LastPosition)
				{
				/* Run all over the value till last char */
				while(i<LastPosition)
				{
					/* Set value in array */
					DataArray[DC++] = (int)string[i];
					i++;
				}
				/* Sign end of string */
				DataArray[DC++] = 0;
			}
			else
			{
				printf("Error! Line number : %d you did not close command .string \n", countLine );
			}
		}
		else
		{
			printf("Error! Line number : %d , illegal character %c \n", countLine, string[i] );
		}
	}
	else
	{
		printf("Error! Line number %d , there is no data string \n",countLine);
	}
}

/* This function handle in Guidance : .Extern */
void HandleExtern(char* label)
{
	/* Set label */
	SetLabel(label,strlen(label),(int)CODE,(int)1,(int)0,(int)0);
}

/* This function handle in Guidance : .Entry */
void HandleEntry (char* label, instruction* ins)
{
	char *word;
	word=GetWord(label,'\n');
	/* add file instruction */
	AddFileInstruction(ins,word,LABEL,-1,"",0,-1);
	
}

/* This file search within instruction struct and return operand count otherwise -1 ("no such Instruction") */
instruction* InstructionLookup (char *Instruction)
{
	int i;
	for (i=0;i<MAX_INSTRUCTION;i++)
	if (strcmp(Instruction,constInstructionArr[i].name) == 0)
		return &constInstructionArr[i];

	/* Return address */
	return &constInstructionArr[MAX_INSTRUCTION];
}

/* This function identify Method of addressing */
int IdentifyOperandType (char *operand)
{
	if(operand[0] == '#')
		return IMMEDIATE;
	else if (RegisterLookup(operand))
		return REGISTER;
	else
	{
		return CheckOperand(operand);
	}

}

/* This function checks number of register and return its number */
int WhichRegister (char *_register,int model)
{
	int i,j;
	char *registryName;

	if(model==REGISTER)
	{
		for (i=0;i<MAX_REGISTER;i++)
			if(strcmp(_register,strRegister[i].name)==0)
			{
				return i;
			}
	}
	if(model==LABEL_PLUS_LABEL_PLUS_REGISTER)
	{
		for(i=0;i<MAX_LINE;i++)
			if(_register[i]=='}')
				break;
		for(j=i;j<MAX_LINE;j++)
			if(_register[j]=='{')
				break;
		registryName=GetWord(&_register[j+1],'}');
				
		for (i=0;i<MAX_REGISTER;i++)
			if(strcmp(registryName,strRegister[i].name)==0)
			{
				return i;
			}
	
	}
	return -1;
}

int checkType(char* line, int countLine, instruction* ins,char *command)
     {  int i=0;
	while(isspace(line[i]))
		i++;
	if((line[i]!=' ') && (line[i]!=',') && (line[i]!='/') && (line[i]!='0') && (line[i]!='1'))
		return 0;
	if(line[i]=='0') 
		{  ins->type =0; 
		   i++;	
		   if((line[i]!=' ') && (line[i]!=',') && (line[i]!='0') && (line[i]!='1'))
			return 0;

		   if(line[i]==' '|| line[i]==',')
			i++;
		   else 
		   	return 0;

    		  if(line[i]!='1' && (line[i]!='0'))
		    {   printf("Error! in line %d :%s/%s  illegal DBL operand\n",countLine,command,line);
			return 0;
		    }
		   if(line[i]=='0')
			ins->DBL=0; 
		   if(line[i]=='1')
			ins->DBL=1;
		   return 1;
		}


	if(line[i]=='1')
	     {	 ins->type =1;
		 i++;
		 if((line[i]!='/') || (line[i+2]!='/'))
		    return 0;	
		 i++;
		 if(line[i]=='1')
			ins->TypeVal[0]=1;  
		 if(line[i]=='0')
			ins->TypeVal[0]=0; 
		 if((line[i]!='0') && (line[i]!='1'))
			{ printf("Error! in line %d :%s%s   invalid first operand \n",countLine,command,line); 
			  return 0;
			}
		 i+=2;
		 if(line[i]=='1')
			ins->TypeVal[1]=1;  
		 if(line[i]=='0')
			ins->TypeVal[1]=0;  
		 if((line[i]!='0') && (line[i]!='1'))
			{ printf("Error! in line %d :%s%s   invalid second operand \n",countLine,command,line); 
			  return 0;
			}
		  i++;
		  return 1;
		 if((line[i]!=' ') && (line[i]!=',') && (line[i]!='0') && (line[i]!='1'))
			return 0;
		 if(line[i]==',')
			i++;
    		 if(line[i]!='1' && (line[i]!='0'))
		  {  {   printf("Error! in line %d :%s/%s  illegal DBL operand\n",countLine,command,line);
			return 0;
		    }
		   if(line[i]=='0')
			ins->DBL=0;
		   if(line[i]=='1')
			ins->DBL=1; 
		   return 1;}
		}
	else 
	{       printf("illegal Type operand\n");
		return 0;
 		}
 }
