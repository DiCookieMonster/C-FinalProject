/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 /*
 * symbolManage.c
 * Managing of the symbols collected during the phase1.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbolManage.h"
#include "global.h"
#include "dispose.h"

SymbolNode* symbList=NULL;
SymbolNode* last=NULL;
extern int countLine;

/* This function adds new symbol to symbolList */
int AddSymbol (char* name, int type, int isExtern, int isEntry, int address)
{
	/* check if symbol is not exist */
    if (GetSymbol(name) != NULL)
        return 0;
    else
    {
        SymbolNode* newNode = (SymbolNode*)calloc(1, sizeof(SymbolNode));
        Symbol* symb = (Symbol*)calloc(1, sizeof(Symbol));

        symb->name = name;
        symb->type = type;
        symb->isExtern = isExtern;
        symb->isEntry = isEntry;
        symb->address = address;

        newNode->symbol = symb;

        if (symbList == NULL)
            symbList = newNode;
        else
            last->nextNode = newNode;
        last = newNode;
    
        return 1;
    }
}

/* This function checks if the symbol exist */
Symbol* GetSymbol (char* name)
{   char* temp=strtok(name,"{*");
    SymbolNode* currentNode = symbList;
	
    while (currentNode != NULL)
    {
        if (strcmp(currentNode->symbol->name, temp) == 0)
		{
				return currentNode->symbol;
		}
		else
		{
			currentNode=currentNode->nextNode;
		}
    }

    return NULL;
}

/* This function free all symbols */
void FreeSymbols ()
{
    SymbolNode* currentNode = symbList;
    while (currentNode != NULL)
    {
        SymbolNode* temp;
        free(currentNode->symbol->name);
        free(currentNode->symbol);

        temp = currentNode->nextNode;
        free(currentNode);
        currentNode = temp;
    }
    symbList = NULL;
    last = NULL;
}

/* This function insert new label to symbol list */
void SetLabel (char *line,int len,int type,int isExtern, int isEntery , int address)
{
	char *label = (char*) calloc(len +1, sizeof(char));
	int i,j=0;
	int status=0;

	if(len>0)
	{
		for(i=0;i<len;i++)
		{
			/* check if not space and end of string label */
			if(!(isspace(line[i])) && line[i]!=':')
				label[j++]=line[i];
		}
		label[j]='\0';
		if(CheckLabelNameLegality(label))
		{
			status=AddSymbol(label,type,isExtern,isEntery,address);
			if(!(status))
			{
				printf("Error! in Line %d:%s  label %s has already been added\n", countLine, line, label);
				FirstPhaseSuccess=0;
			}
		}
		else
		{
			printf("Error! in Line %d:%s  %s is register name or instruction name \n", countLine, line, label);
			FirstPhaseSuccess=0;
		}
	}
}
