#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#define LEN 256

char * getInput(FILE*, int);
int * createShiftTable(char*, char);
int horspoolMatching(char*, char*, char, int*, int);
void replace(char*, char*, char**, char, int*, int);
int isEqual(char, char, char);

int main()
{
	FILE *fp;
 	time_t start, stop;
	int *badMatch, *loc, count, i;
	char method, *text, *findStr, *replaceStr, sense, fileName[LEN];

	printf("Enter the filename : ");
	scanf("%s", fileName);
	
	printf("Find : ");
	findStr = getInput(stdin, '\n');

	printf("Replace : ");
	replaceStr = getInput(stdin, '\n');

	printf("Option : ");
	sense = getchar();

	start = time(NULL);

	fp = fopen(fileName, "a+");
	text = getInput(fp, EOF);

	printf("Text : %s\n", text);

	badMatch = createShiftTable(findStr, sense);

	count = 0;
	loc = (int*) malloc(sizeof(int));
	while( ( loc[count] = horspoolMatching(findStr, text, sense, badMatch, loc[count]+1) ) != -1 )
	{
		count++;
        	loc = (int*) realloc(loc, (1+ count) * sizeof(int));
	}

    i = 0;
    for(; i < count; i++)
        replace(findStr, replaceStr, &text, sense, badMatch, loc[i]);

	fprintf(fp, "%s", text);

	printf("Found and Replaced : %d\n", count);
	
	stop = time(NULL);

	printf("Running time : %lf\n", (double)difftime(stop, start));
	
	fclose(fp);

	return 0;
}	

char * getInput(FILE *fp, int c)
{
	/**
	 * Reads the given stream fp,
	 * 	fp may be a file as well as stdin (standard input)
	 * 
	 * :param fp: File Pointer from which the function reads.
	 * :param c: The terminator character, the text will be read until c is reached.
	 * :return str: Returns what has been read as char array.
	 * 
	 */

	char ch, *str = malloc(sizeof(char));
	int ind = 0;
	
	if(fp == stdin)
		scanf(" ");

	while( (str[ind++] = fgetc(fp)) != c )
		str = realloc(str, (1+ind) * sizeof(char));
	str[ind-1] = '\0';

	return str;

}

int * createShiftTable(char *find, char sense)
{
	/**
	 *  Calculates the Shift Table for Boyer-Moore-Horspool algorithm.
	 * 
	 *  :param find: The string which will be found.
	 *  :param sense: Whether the search is case sensitive or not.
	 *  :return table: The table of characters. Each character has its own number.
	 */

	int i = 0, *table, size = strlen(find);
	table = (int*) malloc(LEN * sizeof(int));

	for(; i < LEN; i++)
		table[i] = size;

	for(i = 0; i < size - 1; i++)
	{
		if(sense == 'C' || sense == 'c')
			table[find[i]] = size - 1 - i;
		else
		{
			table[tolower(find[i])] = size - 1 - i;
			table[toupper(find[i])] = size - 1 - i;
		}
		
	}
	return table;
}

int horspoolMatching(char *pattern, char *text, char sense, int *table, int startpos)
{
	/**
	 *  Searchs string "pattern" in text "text".
	 *  
	 *  :param pattern: The string which will be searched in the text.
	 *  :param text: The string in which the pattern will be found.
	 *  :param sense: Whether the search is case sensitive or not.
	 *  :param table: Bad Match Table
	 *  :return index: The start position of the pattern in the text
	 */

	int lenPattern, len_Text, i, k, ind = 0;
	lenPattern = strlen(pattern);
	len_Text = strlen(text);
	i = startpos + lenPattern - 1;

	while (i < len_Text)
	{
		k = 0;

		while( k < lenPattern && isEqual(pattern[lenPattern - 1 - k], text[i - k], sense) )
			k++;

		if(k == lenPattern)
			return i - lenPattern + 1;
		else i += table[text[i]];
	}

	return -1;
}

void replace(char *find, char *replace, char **text, char sense, int *table, int startpos)
{
	/**
	 *  Replaces text "replace" where string "find" has been found.
	 *  
	 *  :param find: The string which will be searched in the text.
	 *  :param replace: The string which will be replaced.
	 *  :param text: The text in which some another string will be found and replaced.
	 *  :param sense: Whether the search is case sensitive or not.
	 *  :param table: Bad Match Table
	 *  :NO RETURN: 
	 */

	int i, start, stop, extra, lenReplace = strlen(replace), lenFind = strlen(find);

	extra = lenReplace - lenFind;
	
	*text = (char*) realloc(*text, (strlen(*text) + extra) * sizeof(char));

	stop = startpos + strlen(find);

	if( extra > 0 )
	{
		for(i = strlen(*text); i >= stop; i--)
			(*text)[i + extra] = (*text)[i];
	}
	else
	{
		for(i = startpos + lenReplace; i < strlen(*text); i++)
			(*text)[i] = (*text)[i - extra];
	}
		
	for(i = 0; i < strlen(replace); i++)
		(*text)[startpos + i] = replace[i];

	(*text)[strlen(*text)] = '\0';
}

int isEqual(char ch1, char ch2, char sense)
{
	/**
	 *  Compares two characters 
	 *  
	 *  :param ch1: The first character which will be compared.
	 *  :param ch2: The second character which will be compared.
	 *  :param sense: Whether the comparison is case sensitive or not. 
	 *  :return equality: Returns 1 if the characters are equal, otherwise 0.
	 */

	if( sense == 'C' || sense == 'c')
		return ch1 == ch2;
	return tolower(ch1) == tolower(ch2); 
}
