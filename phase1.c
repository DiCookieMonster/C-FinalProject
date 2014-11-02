/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 /*
 * phase1.c
 * Managing the first phase of assembly file processing.
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "dispose.h"

void Dispose ();
int CheckLabel (char *line);
int CheckIfVirtualInstruction(char *line , int isLabel);
int CheckIfGuidance(char *line,int isLabel);
void  ManualLine(char *line);
void CheckIfSimpleInctruction (char *line , int isLabel);


/* Count of current processing line */
int countLine;

/* This function manage file and send line to Analysis */
void RunPhase1 (FILE *file)
{
	char line[MAX_LINE+1];
	int i;
	FirstPhaseSuccess=0;
	SeconnedPhaseSucess=0;
	/* Count line */
	/* Send to dispose function for dispose all global variables */
	Dispose();
	/* Get first line */
	fgets(line,MAX_LINE,file);
	/* Go over all file and send each line to analysis */
	while (!(feof(file)))
	 {
		 i=0;
		 countLine++;
		 /* check empty line */
		 while(isspace(line[i]))
			i++;
		 /* check empty line or note line */
		 if(line[i]!='\0' && line[i]!=';' )
			 /* send to analysis */
				ManualLine(line); 	
					 
		 /* get  line */
		fgets(line,MAX_LINE,file);
	 }
}
/* This function manage a single line 
send to Functions : 
1.CheckLabel - check if in line you have label
2.CheckIfVirtualInstruction - check if in line exist command .string or . data
3.CheckIfGuidance - check if in line exist command .extern or . entry 
4.CheckIfSimpleInctruction - for a simple command's */

void  ManualLine(char *line)
{
	int isLabel;
	int is_VirtualInstruction;
	int is_Guidance;
	is_Guidance=isLabel=is_VirtualInstruction=0;

	isLabel=CheckLabel(line);
	is_VirtualInstruction = CheckIfVirtualInstruction(line,isLabel);
	
	if(!(is_VirtualInstruction))
	{	
		is_Guidance=CheckIfGuidance(line,isLabel);
		if(!(is_Guidance))
		{	
			CheckIfSimpleInctruction(line,isLabel);
		}
	}

}



