/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 /*
 * phase2.c
 * Managing the second phase of assembly file processing.
 * After the end of RunPhase2 the output files should be the .ob,.ent,.ext files.
 * In case of a processing error, none of these files will appear.
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "dispose.h"
#include "fileInstruction.h"
#include "stringops.h"
#include "symbolManage.h"
#include <limits.h>

/* Special Sturctures */
/* This struct represents the 20 bit instruction*/
/* Assuming here that unsigned int has the length of 20 bits*/
typedef struct
{	unsigned int type:1;
	unsigned int typeVal:2;
	unsigned int DBL:1;
	unsigned int destRegister:3;
	unsigned int destOperandType:3;
	unsigned int sourceRegister:3;
	unsigned int sourceOperandType:3;
	unsigned int instructionCode:4;
} instructionWord;

/* Represents an external symbol*/
/* A couple of the external symbol and it's referenced address in file*/
typedef struct
{
	Symbol *sym;
	unsigned int refAddress;
} externalSymbol;

/* internals */
/* This is used to collect external symbols during phase 2*/
int externalCounter;
externalSymbol externalArr[MAX_ARRAY_LENGTH];

/* This is used to collect entry symbols during phase 2*/
int entryCounter;
Symbol* entryArr[MAX_ARRAY_LENGTH];

/* Externals */
extern int FIC;
extern int DC;
extern int IC;
extern fileInstruction fInsArr[MAX_ARRAY_LENGTH];
extern int DataArray[MAX_ARRAY_LENGTH];

/* Function signatures */
void WriteOpenningLine (FILE* objFile);
void WriteDataLength (FILE* objFile);
void WriteProgramLength (FILE* objFile);
void WriteInBase8 (FILE* file, int num, int wantedLength);
void AppendSymbol (FILE* objFile, Symbol* sym, unsigned int fileAddr);
void AppendSymbol_STAR (FILE* objFile, Symbol* sym, unsigned int fileAddr);
void AddExternalSymbol (Symbol *sym, unsigned int refAddr);
int WriteEntryFile (char fileName[MAX_FILE_NAME]);
void WriteExternalFile(char fileName[MAX_FILE_NAME]);
void AddEntrySymbol (Symbol *sym);
Symbol* GetNextEntrySymbol (int lastAddress);
FILE* CreateTypedFile (char fileName[MAX_FILE_NAME], char type[MAX_FILE_NAME]);
void DropPhase2(char fileName[MAX_FILE_NAME]);
void WriteDataToFile (FILE* objFile, int startAddr);
int WriteInstruction (FILE* objFile, fileInstruction fins, int startAddr);
int AppendOperandWords (FILE* objFile, operand op, int startAddr);

/* Main flow for Phase 2 of file processing*/
void RunPhase2 (char fileName[MAX_FILE_NAME])
{
	int currentIC = START_ADDRESS, i;
	FILE* objFile;
	char *fileNameWithoutExt = strtokEx(fileName, ".", 0); /* Extracting the file name without extension*/

	/* Creating a file with the .ob extension*/
	objFile = CreateTypedFile(fileNameWithoutExt, ".ob");

	externalCounter = 0;
	entryCounter = 0;
	if (objFile == NULL)
	{
		fprintf(stderr, "Could not open object file for writing.\n");
		SeconnedPhaseSucess=0;
		return;
	}

	/* Write the openning line which includes the instruction count and data count (in words).  */
	WriteOpenningLine(objFile);

	/* Iterate all instructions saved in Phase 1.  */
	for (i = 0; i < FIC; i++)
	{
		int wordsAdded = 0;

		fileInstruction fins = fInsArr[i];

		/* Handling .entry instructions later.  */
		if (strcmp(fins.ins->name, ".entry") == 0)
			continue;
		
		/* Writing the current instruction and getting the number of words added to the file afterwards.  */
		wordsAdded = WriteInstruction(objFile, fins, currentIC);
		if (!wordsAdded) /* This means an error occured during the instruction write.  */
		{
			fprintf(stderr, "Error while processing file instruction on address:");
			SeconnedPhaseSucess=0;
			WriteInBase8(stderr, currentIC, 4);
			fputc('\n', stderr);
			fclose(objFile);

			/* Stop phase 2 and delete the .ob file.  */
			DropPhase2(fileNameWithoutExt); 
			free(fileNameWithoutExt);
			return;
		}

		/* Increase the instruction counter with the number of words added last time*/
		currentIC += wordsAdded;
	}

	/* At last, append all data from the DataArray into the file*/
	WriteDataToFile(objFile, currentIC);

	/* Write the entry file*/
	/* Possible error is that the entry instruction points towards an undefined symbol*/
	success = WriteEntryFile(fileNameWithoutExt);
	if (!success)
	{	
		fclose(objFile);
		DropPhase2(fileNameWithoutExt);
		free(fileNameWithoutExt);
		return;
	}

	/* Write the external file (If one is needed)*/
	WriteExternalFile(fileNameWithoutExt);

	fclose(objFile);
	free(fileNameWithoutExt);
}

/* Appends the DataArray into the .ob file*/
void WriteDataToFile (FILE* objFile, int startAddr)
{
	int i;
	for (i = 0; i < DC; i++)
	{
		WriteInBase8(objFile, startAddr++, 4);
		fputc('\t', objFile);
		WriteInBase8(objFile, DataArray[i], 8);
		fputc('\n', objFile);
	}
}

/* Prints a phase2 error*/
/* Removes the .ob file*/
void DropPhase2(char fileName[MAX_FILE_NAME])
{
	char fileNameTyped[MAX_FILE_NAME];
	fprintf(stderr, "Could not complete Phase2.\n");
	SeconnedPhaseSucess=0;
	strcpy(fileNameTyped, fileName);
	strcat(fileNameTyped, ".ob");
	remove(fileNameTyped);
}

/* Writes the External file*/
void WriteExternalFile(char fileName[MAX_FILE_NAME])
{
	FILE* extFile;
	int i;

	if (externalCounter == 0)
		return;
	/* Creating the .ext file*/
	extFile = CreateTypedFile(fileName, ".ext");

	if (extFile == NULL)
	{
		fprintf(stderr, "Could not create the Externals file for \"%s.as\"\n", fileName);
		SeconnedPhaseSucess=0;
		return;
	}

	/* Iterating all external symbols*/
	for (i = 0; i < externalCounter; i++)
	{
		externalSymbol exSym = externalArr[i];

		/* Writing the external symbol*/
		fprintf(extFile, "%s\t", exSym.sym->name);

		/* Writing the external referenced address*/
		WriteInBase8(extFile, exSym.refAddress, 0);
		fputc('\n', extFile);
	}

	fclose(extFile);
}

/* Writes the Entry file*/
int WriteEntryFile (char fileName[MAX_FILE_NAME])
{
	int i;
	Symbol *lastSymbol = NULL;
	FILE* entFile;

	/* Mark relevant symbols as entries*/
	/* This action is done by iterating all .entry instructions alone*/
	for (i = 0; i < FIC; i++)
	{
		Symbol *sym;

		fileInstruction fins = fInsArr[i];
		if (strcmp(fins.ins->name, ".entry") != 0)
			continue;

		sym = GetSymbol(fins.op1.name);

		if (sym == NULL)
		{
			fprintf(stderr, "Could not find symbol for entry: %s\n", fins.op1.name);
			SeconnedPhaseSucess=0;
			return 0;
		}

		/* Collecting this symbol*/
		AddEntrySymbol(sym);
	}

	/* If no entries were found, return in success right now*/
	if (!entryCounter)
		return 1;

	/* Create the .ent file*/
	entFile = CreateTypedFile(fileName, ".ent");

	if (entFile == NULL)
	{
		fprintf(stderr, "Could not create the Entries file for \"%s.as\"\n", fileName);
		SeconnedPhaseSucess=0;
		return 0;
	}

	/* Get the first entry symbol*/
	lastSymbol = GetNextEntrySymbol(-1);

	/* Write all entry symbols in ascending order (by address)*/
	do
	{
		int addr = (lastSymbol->address);
		if (lastSymbol->type == DATA) /* If it's a DATA symbol, add the IC to it*/
			addr += IC;

		fprintf(entFile, "%s\t", lastSymbol->name);
		WriteInBase8(entFile, (addr+START_ADDRESS), 0);
		fputc('\n', entFile);

		/* Get the next symbol*/
		lastSymbol = GetNextEntrySymbol(addr);
	} while (lastSymbol != NULL);

	fclose(entFile);
	return 1;
}

/* Writes an instruction to the .ob file*/
/* Returns the number of words added to the file*/
/* If an error was found, a message is printed and 0 is returned*/
int WriteInstruction (FILE* objFile, fileInstruction fins, int startAddr)
{
	int length = 1;

	instructionWord iw; /* This struct represents the instruction in 20 bits (assuming unsigned int is 20 bits)*/

	/* first, append the starting address*/
	WriteInBase8(objFile, startAddr, 4);
	fputc('\t', objFile);

	iw.instructionCode = fins.ins->code;
	iw.type = fins.ins->type;
	iw.typeVal = fins.ins->TypeVal[0];
	iw.DBL = fins.ins->DBL;
	iw.sourceOperandType = fins.op1.operandType;
	iw.sourceRegister = fins.op1.reg;
	iw.destOperandType = fins.op2.operandType;
	iw.destRegister = fins.op2.reg;

	/* Cast this instructionWord into unsigned int and append it to the file in base 8*/
	WriteInBase8(objFile, *((unsigned int*)&iw), 8);
	fputs("\ta\n", objFile);


	/* If this instruction has no operands finished*/
	if (fins.ins->operandCount == 0)
		return length;

	/* Analysing the first operand if it has label/immediate in it*/
	/* Doing so only if the instruction operand count > 1 */
	
	if (iw.sourceOperandType >= 0 && iw.sourceOperandType <= 3 && fins.ins->operandCount > 1)
	{
		int wordsAdded = 0;

		/* Appending the operand words that we need*/
		/* Getting the appended words count*/
		wordsAdded = AppendOperandWords(objFile, fins.op1, ++startAddr);
		startAddr = (startAddr + wordsAdded - 1);
		if (wordsAdded == 0) /* Error? */
			return 0;
		length += wordsAdded;
	}

	/* Analysing the second operand if it has label/immediate in it*/
	/* Note: same as above*/
	if (iw.destOperandType >= 0 && iw.destOperandType <= 3) 
	{
		int wordsAdded = 0;
		wordsAdded = AppendOperandWords(objFile, fins.op2, ++startAddr);

		if (wordsAdded == 0)
			return 0;
		length += wordsAdded;
	}

	return length;
}

/* Appends an operand into file*/
/* The function should get only an operand whose type is 0 to 3 (contains label/immediate)*/
/* Returns the number of words appended (0 = error)*/
int AppendOperandWords (FILE* objFile, operand op, int startAddr)
{
	int imm;
	int addr = startAddr;
	char *label1, *label2;
	Symbol *sym1, *sym2;

	/* First, write the starting address for the first symbol*/
	WriteInBase8(objFile, startAddr, 4);
	fputc('\t', objFile);

	/* What is the operand type? */
	switch (op.operandType)
	{
	case IMMEDIATE:
		label1 = strtokEx(op.name, "#", 0);
		if (label1 == NULL) { return 0; }

		imm = atoi(label1); /* Convert the immediate string to int*/
		WriteInBase8(objFile, imm, 8); /* Write it in base 8 */
		fputs("\ta\n", objFile);
		free(label1);
		return 1;
	case LABEL:
		sym1 = GetSymbol(op.name); /* Get the wanted symbol*/
		if (sym1 == NULL)
		{
			fprintf(stderr, "Could find symbol: %s\n", op.name);
			SeconnedPhaseSucess=0;
			return 0;
		}
		AppendSymbol(objFile, sym1, addr); /* Append symbol to file*/ 
		return 1;
	case LABEL_PLUS_STAR_LABEL:
		label1 = strtokEx(op.name, "{}", 0); /* Getting the first label (host)*/
		label2 = strtokEx(op.name, "{}", 1); /* Getting the second label (offset)*/
		
		label2 = strcpy(label2,label2+1);
		sym1 = GetSymbol(label1), sym2 = GetSymbol(label2);
		

		if (sym1 == NULL) { 
			fprintf(stderr, "Could not find symbol: %s\n", label1); free(label1); 
			free(label2); 
			SeconnedPhaseSucess=0;			
			return 0; 
		}
		if (sym2 == NULL) { 
			fprintf(stderr, "Could not find symbol: %s\n", label2); free(label1); 
			free(label2); 
			SeconnedPhaseSucess=0;
			return 0; 
		}

		/* Append first symbol*/
		AppendSymbol(objFile, sym1, addr);

		addr = ++startAddr;

		/* We started a new line, write the next address*/
		WriteInBase8(objFile, addr, 4);
		fputc('\t', objFile);

		/* Append the second symbol*/
		AppendSymbol_STAR(objFile, sym2, addr);
		
		
		
		free(label1);
		free(label2);
		return 2;
	case LABEL_PLUS_LABEL_PLUS_REGISTER:
		label1 = strtokEx(op.name, "{}", 0); /* Get the label (host)*/ 
		label2 = strtokEx(op.name, "{}", 1); /* Get the second label (host)*/ 

		sym1 = GetSymbol(label1);
		sym2 = GetSymbol(label2);
		if (sym1 == NULL) { 
			fprintf(stderr, "Could find symbol: %s\n", label1);  
			free(label1); 
			SeconnedPhaseSucess=0;
			return 0; 
		}
		if (sym2 == NULL) { 
			fprintf(stderr, "Could find symbol: %s\n", label2);  
			free(label2);
			SeconnedPhaseSucess=0; 
			return 0; 
		}
		
		AppendSymbol(objFile, sym1, addr);
		WriteInBase8(objFile, ++startAddr, 4);
		fputc('\t', objFile);
		AppendSymbol(objFile, sym2, addr);

		free(label1);
		return 2;
	default:
		return 0;
	}
}

/* Appends a symbol into the file*/
void AppendSymbol (FILE* objFile, Symbol* sym, unsigned int fileAddr)
{
	/* If this is a data symbol we should add the IC to it's address*/
	if (sym->type == DATA)
		WriteInBase8(objFile, IC + sym->address+START_ADDRESS, 8);
	else
		if (!sym->isExtern)
			WriteInBase8(objFile, (sym->address+START_ADDRESS-1), 8);
		else
			WriteInBase8(objFile, (sym->address), 8);
	/* Checking if this symbol is extern*/
	if (sym->isExtern)
	{
		fputs("\te\n", objFile);

		/* Collecting this external symbol with it's referenced address*/
		AddExternalSymbol(sym, fileAddr);
	}
	else
		fputs("\tr\n", objFile);
}
void AppendSymbol_STAR (FILE* objFile, Symbol* sym, unsigned int fileAddr)
{
	/* If this is a data symbol we should add the IC to it's address*/
	if (sym->type == DATA)
	{
		if (fileAddr > sym->address)
		{
			WriteInBase8(objFile, (IC + sym->address+START_ADDRESS)-(fileAddr-2), 8);
		}
		else
			WriteInBase8(objFile, (IC + sym->address+START_ADDRESS)-(fileAddr-2), 8);
	}
	else
		if (!sym->isExtern)
		{
			if (fileAddr > (sym->address+START_ADDRESS))
				WriteInBase8(objFile, abs((sym->address+START_ADDRESS)-(fileAddr-1)), 8);
			else
				WriteInBase8(objFile, (sym->address+START_ADDRESS)-(fileAddr-1), 8);
		}
		else
			WriteInBase8(objFile, (sym->address), 8);

	/* Checking if extern*/
	if (sym->isExtern)
	{
		fputs("\te\n", objFile);

		/* Collecting this external symbol with it's referenced address*/
		AddExternalSymbol(sym, fileAddr);
	}
	else
		fputs("\ta\n", objFile);
}
/* Collects an external symbol into the externalArr*/
/* This array is a set of an external symbol coupled with it's referenced address in file*/
void AddExternalSymbol (Symbol *sym, unsigned int refAddr)
{
	externalArr[externalCounter].sym = sym;
	externalArr[externalCounter++].refAddress = refAddr;
}

/* Collects an entry symbol into the entryArr*/
void AddEntrySymbol (Symbol *sym)
{
	/* Checking if this symbol was already marked and added*/
	if (sym->isEntry)
		return;

	sym->isEntry = 1;
	entryArr[entryCounter++] = sym;
}

/* This function returns the next entry symbol out of the entryArr*/
/* The symbol returned is in address which is bigger than the lastAddress given*/
Symbol* GetNextEntrySymbol (int lastAddress)
{
	int minAddr = MAX_ARRAY_LENGTH;
	Symbol *minSymbol = NULL;
	int i;

	/* Finding the minimum addressed symbol whose address is bigger than lastAddress which was given to the function*/
	for (i = 0; i < entryCounter; i++)
	{
		Symbol *sym = entryArr[i];
		int addr = sym->address;

		if (sym->type == DATA)
			addr += IC;

		if (addr < minAddr && addr > lastAddress)
		{
			minAddr = addr;
			minSymbol = sym;
		}
	}

	return minSymbol;
}

/* Writes the openning line for the file - instruction count and data count (in words)*/
void WriteOpenningLine (FILE* objFile)
{
	WriteProgramLength(objFile);
	fputc(' ', objFile);
	WriteDataLength(objFile);
	fputc('\n', objFile);
}

/* Writes the data length in words*/
void WriteDataLength (FILE* objFile)
{
	WriteInBase8(objFile, DC, 0);
}

/* Writes the instruction count in words*/
void WriteProgramLength (FILE* objFile)
{
	WriteInBase8(objFile, IC, 0);
}

/* This function writes to file a given number in base 8*/
/* You can also ask for a wanted length for the converted number and it will be padded with zeros*/
void WriteInBase8 (FILE* file, int num, int wantedLength)
{
	int mod = 0, div = num, missing, i;
	char finalStr[START_ADDRESS];

	/* Converting to base 8 while collecting the characters into finalStr*/
	finalStr[0] = 0;
	do
	{
		char str[3];
		mod = div % 8;
		div = div / 8;
		if (mod==10) 
		{
			sprintf(str,"%c",'a');
		}
		else if (mod==11) 
		{
			sprintf(str,"%c",'b');
		}
		else sprintf(str, "%d", mod);
		strcat(finalStr, str);
	} while (div > 0);

	/* Pad this string  */
	missing = wantedLength - strlen(finalStr);
	while (missing-- > 0)
	{
		fputc('0', file);
	}

	/* Writing this string into the file backwards*/
	for (i = strlen(finalStr)-1; i >= 0; i--)
	{
		fputc(finalStr[i], file);
	}
}




/* Attemps to create a file with a specific extension*/
FILE* CreateTypedFile (char fileName[MAX_FILE_NAME], char type[MAX_FILE_NAME])
{
	char tFileName[MAX_FILE_NAME];
	FILE* file;

	strcpy(tFileName, fileName);
	strcat(tFileName, type);
	file = fopen(tFileName, "w");

	return file;
}

