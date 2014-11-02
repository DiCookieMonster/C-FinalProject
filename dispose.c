/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================

 */
 
#include <stdio.h>
#include "dispose.h"


/* This function dispose all global variables */
void Dispose ()
{
	DC=0;
	FIC=0;
	IC=0;
	countLine=0;
	FreeSymbols();
}
