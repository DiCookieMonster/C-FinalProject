/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "dispose.h"
#include "stringops.h"

int FirstPhaseSuccess;
int SeconnedPhaseSucess;
/* dispose all global variables. */
void Dispose();
void ProcessFile (char* filePath);
void RunPhase1 (FILE *file);
int CheckLegalFileName (char *fileName);
void RunPhase2 (char fileName[MAX_FILE_NAME]);



int main(int argc, char **argv)
{
		int i;

		puts("=================");
		puts("Welcome!");
		puts("all rigths reserved to Dina Suranovich");
		puts("Loading........");
		puts("Pay attantion the application is case sensitive !");
		puts("the application will not accept invalid format or extra tabs in the main command(between the Type and DBL)");
		puts("=================");

		i = 1;
		while (i < argc)
		{
			ProcessFile(argv[i]);
			i++;
		}
        exit(EXIT_SUCCESS);
}

/* Take care about a file */
void ProcessFile (char* filePath)
{
    	FILE* file;
	FirstPhaseSuccess=0;
	SeconnedPhaseSucess=1;
    /* Check legality of a file name */
	if(CheckLegalFileName(filePath))
	{
		 if (!(file = fopen(filePath, "r")))
		 {
			  puts("----------------");
			  fprintf(stderr, "Error! :Can't open the file %s.\n", filePath);
			  puts("----------------");
			  return;
		  }

		printf("Processing file %s\n", filePath);

		fputs("Processing phase 1...\n", stdout);
 	    RunPhase1(file);
	    fclose(file);

	    /* FirstPhaseSuccess is a global Variables that use a flag if everything is fine in phase 1 */
		if(FirstPhaseSuccess==1)
		{
			fputs("Processing phase 2...\n", stdout);
			RunPhase2(filePath);

			/* Dispose global variables each file */
			Dispose();
		}
		else
		{
			puts("----------------");
			puts("compilation error  , there are some syntax errors in the file");
			puts("----------------");
			/* Dispose global variables in each file */
			Dispose();
			return;
		}

		}
	else
	{
		printf("Error! In file %s  , The program supports assembly files only \n" , filePath);
		puts("----------------");
		return;
	}
	if (SeconnedPhaseSucess==0 && FirstPhaseSuccess==1)
		{ puts("succesfully compiled");
          return;
		}
	else
		{ puts("prosses terminated pleas check your assembly file");
		  Dispose();
		  return;
		  }

}

/* This function check if the file is .as file */
int CheckLegalFileName (char *fileName)
{
	int result;
	char *ext;
	ext = strtokEx(fileName, ".", 1);

	if (ext == NULL)
		return 0;

	if (strcmp(ext, "as") != 0)
		result = 0;
	result = 1;

	free(ext);
	return result;
}


