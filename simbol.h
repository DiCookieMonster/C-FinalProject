/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 
#include <stdlib.h>


enum symbolTypes { DATA, CODE };

/* This struct represents symbol */
typedef struct
{
	char* name;
	unsigned int type;
	unsigned int isExtern;
	unsigned int isEntry;
	unsigned int address;
}Symbol;

/* This struct represents node of symbol */
typedef struct SymNode
{
    Symbol* symbol;
    struct SymNode *nextNode;
}SymbolNode;

