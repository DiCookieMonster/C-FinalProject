/*
 =============================================================
 program    : final project Systems Programming Laboratory (20465)
 Author     : Dina Suranovich
 =============================================================
 */
 
 #include "stringops.h"

/* This function is a custom realization of strtok. */
/* It gets an index of a wanted word out of the splitted expression. */
/* Returns NULL if the index is too big for the expression. */
char* strtokEx (char* str, char* seps, int index)
{
    char* ret = (char*)calloc(MAX_STRING_LENGTH, sizeof(char)), strCopy[MAX_STRING_LENGTH];
    char* temp;
    int i = 0;

	strcpy(strCopy, str);

    /* Start tokenising the string until you get what you want. */
    temp = strtok(strCopy, seps);
    while (temp != NULL && i <= index)
    {
        strcpy(ret, temp);
        temp = strtok(NULL, seps);

        i++;
    }

    /* Finished parsing the string before the requested index was found. */
    if (i <= index)
    {
    	free(ret);
        return NULL;
    }
    else
        return ret;
}


