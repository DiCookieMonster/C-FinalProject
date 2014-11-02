/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* This function gets a line and returns word till 'stop' character */
char *GetWord (char *line , char stop)
{
	char *word=(char*) malloc (strlen(line)*sizeof(char));
	int i=0;/* for line */
	int j=0;/* for word */
	/* ignore all spaces */
	while(isspace(line[i]))
		i++;
	i=0;
	while (line[i] != stop && line[i]!='\n')
	{
		if(!(isspace(line[i])))
			word[j++]=line[i++];
		else
			i++;
	}
	word[j]='\0';

	if(j>0)
	{
		return word;
	}
	else
	{
		return "";
	}
}

/* This function checks if all characters of num is digits */
int IsDigitCustom (char* num)
{
	int i=0;
	int len = strlen(num);

	while (i<len)
	{
		if(!(isdigit(num[i++])))
			return 0 ;
	}

	return 1;

}
