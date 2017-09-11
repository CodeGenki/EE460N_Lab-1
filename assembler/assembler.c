/*
Name 1: Ashkan Vafaee
Name 2: Genki Oji
UTEID 1: av28837
UTEID 2: gto99
*/

#define _CRT_SECURE_NO_WARNINGS		/* Remove this Later */
#define MAX_LINE_LENGTH 255
#include <stdio.h>	/* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h>	/* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

enum
{
	DONE, OK, EMPTY_LINE
};

/* Protoypes of functions just in case */
int readAndParse(FILE *pInfile, char *pLine, char **pLabel, char **pOpcode, char **pArg1, char **pArg2, char **pArg3, char **pArg4);
int toNum(char * pStr);
int isOpcode(char *lPtr);
void pseudoOp(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void add(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void and(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void br(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void halt(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void jmp(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void jsr(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void jsrr(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void ldb(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void ldw(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void lea(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void nop(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void not(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void ret(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void lshf(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void rshfl(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void rshfa(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void rti(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void stb(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void stw(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void trap(char **pArg1, char **pArg2, char **pArg3, char **pArg4);
void xor(char **pArg1, char **pArg2, char **pArg3, char **pArg4);




int main(int argc, char* argv[]) {
	char *prgName = NULL;
	char *iFileName = NULL;
	char *oFileName = NULL;
	char *pLine = (char*)malloc(sizeof(char)*MAX_LINE_LENGTH);
	char **pLabel = (char**)malloc(sizeof(char*) * MAX_LINE_LENGTH);
	char **pOpcode = (char**)malloc(sizeof(char*) * MAX_LINE_LENGTH);

	char **pArg1 = (char**)malloc(sizeof(char*) * MAX_LINE_LENGTH);
	char **pArg2 = (char**)malloc(sizeof(char*) * MAX_LINE_LENGTH);
	char **pArg3 = (char**)malloc(sizeof(char*) * MAX_LINE_LENGTH);
	char **pArg4 = (char**)malloc(sizeof(char*) * MAX_LINE_LENGTH);
	char **labels_list = (char*)malloc(sizeof(char*) * MAX_LINE_LENGTH);		/* Holds label names */
	int *address_list = (int*)malloc(sizeof(int) * MAX_LINE_LENGTH);	/* Holds address of each label */
	int start_address;
	FILE* infile = NULL;
	FILE* outfile = NULL;

	/* Acquires Input and Output file names */
	prgName = argv[0];
	iFileName = argv[1];
	oFileName = argv[2];

	printf("program name = '%s'\n", prgName);
	printf("input file name = '%s'\n", iFileName);
	printf("output file name = '%s'\n", oFileName);


	/* open the source file */
	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");

	if (!infile) {
		printf("Error: Cannot open file %s\n", argv[1]);
		exit(4);
	}
	if (!outfile) {
		printf("Error: Cannot open file %s\n", argv[2]);
		exit(4);
	}






	/******************** Parsing Each Line Here ********************/

	/* Getting Starting Address from first line*/
	int status = readAndParse(infile, pLine, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4);

	if (!strcmp(*pArg1, ".orig") && status == OK) {
		start_address = toNum(*pArg2);

		/* NOTE: possibly check max address size for lc3-b ?*/
		if (start_address % 2 == 1) {
			exit(3);					/* odd constant -> invalid constant*/
		}

	}
	else {
		exit(4);		/* No .orig pseudo op */
	}

	status = readAndParse(infile, pLine, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4);
	/******************** First Pass ********************/
	int temp_address = start_address;
	int i = 0;

	while (status != DONE) {
		if (status != EMPTY_LINE && strcmp(*pLabel,"")) {


			if (*pLabel[0] == 'x' || (*pLabel[0] >= '0' && *pLabel[0] <= '9')) {		/* Checking if label starts with 'x' or digit */
				exit(4); /* Illegal Label */
			}

			if (!strcmp(*pLabel, "in") || !strcmp(*pLabel, "out") || !strcmp(*pLabel, "getc") || !strcmp(*pLabel, "puts")) {
				exit(4); /* Illegal Label */
			}

		
			for (int i = 0; i < strlen(*pLabel); i++) {
				if (!((*pLabel[i] >= 'a' && *pLabel[i] <= 'z') || (*pLabel[i] >= '0' && *pLabel[i] <= '9'))) {
					exit(4); /* Illegal Label */
				}
			}

			labels_list[i] = *pLabel;
			address_list[i] = temp_address;
			i++;
		}


		if (status != EMPTY_LINE) {
			temp_address += 2;
		}
		status = readAndParse(infile, pLine, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4);
	}







	fclose(infile);
	fclose(outfile);

}


int readAndParse(FILE *pInfile, char *pLine, char **pLabel, char **pOpcode, char **pArg1, char **pArg2, char **pArg3, char **pArg4){
	char * lRet, *lPtr;
	int i;
	if (!fgets(pLine, MAX_LINE_LENGTH, pInfile))
		return(DONE);
	for (i = 0; i < strlen(pLine); i++)
		pLine[i] = tolower(pLine[i]);

	/* convert entire line to lowercase */
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	/* ignore the comments */
	lPtr = pLine;

	while (*lPtr != ';' && *lPtr != '\0' &&
		*lPtr != '\n')
		lPtr++;

	*lPtr = '\0';
	if (!(lPtr = strtok(pLine, "\t\n ,")))
		return(EMPTY_LINE);

	if (isOpcode(lPtr) == -1 && lPtr[0] != '.') /* found a label */
	{
		*pLabel = lPtr;
		if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	}

	/* Pseudo-Op Found */
	if (lPtr[0] == '.') {
		if (!(strcmp(lPtr, ".orig")) || !(strcmp(lPtr, ".fill")) || !(strcmp(lPtr, ".end"))) {
			*pArg1 = lPtr;
			
			if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

			*pArg2 = lPtr;

			return(OK);
		}
		else {
			/* illegal label */
			exit(4);
		}
	}



	*pOpcode = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg1 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg2 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg3 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg4 = lPtr;

	return(OK);
}


int isOpcode(char *lPtr) {

	if (!(strcmp(lPtr, "add"))	|| !(strcmp(lPtr, "and"))	|| !(strcmp(lPtr, "br"))	|| !(strcmp(lPtr, "brn"))	|| !(strcmp(lPtr, "brz")) ||
		!(strcmp(lPtr, "brp"))	|| !(strcmp(lPtr, "brnz"))	|| !(strcmp(lPtr, "brnp"))	|| !(strcmp(lPtr, "brzp"))	|| !(strcmp(lPtr, "brnzp")) ||
		!(strcmp(lPtr, "halt")) || !(strcmp(lPtr, "jmp"))	|| !(strcmp(lPtr, "jsr"))	|| !(strcmp(lPtr, "jsrr"))	|| !(strcmp(lPtr, "ldb")) ||
		!(strcmp(lPtr, "ldw"))	|| !(strcmp(lPtr, "lea"))	|| !(strcmp(lPtr, "nop"))	|| !(strcmp(lPtr, "not"))	|| !(strcmp(lPtr, "ret")) ||
		!(strcmp(lPtr, "lshf")) || !(strcmp(lPtr, "rshfl")) || !(strcmp(lPtr, "rshfa")) || !(strcmp(lPtr, "rti"))	|| !(strcmp(lPtr, "stb")) ||
		!(strcmp(lPtr, "stw"))	|| !(strcmp(lPtr, "trap"))	|| !(strcmp(lPtr, "xor"))) {

		return 0;
	}


	return -1;

}



void pseudoOp(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void add(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void and(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void br(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void halt(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void jmp(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void jsr(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void jsrr(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void ldb(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void ldw(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void lea(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void nop(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void not(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void ret(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void lshf(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void rshfl(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void rshfa(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void rti(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void stb(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void stw(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void trap(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}
void xor(char **pArg1, char **pArg2, char **pArg3, char **pArg4) {}








int toNum(char * pStr)
{
	char * t_ptr;
	char * orig_pStr;
	int t_length, k;
	int lNum, lNeg = 0;
	long int lNumLong;

	orig_pStr = pStr;
	if (*pStr == '#')                                /* decimal */
	{
		pStr++;
		if (*pStr == '-')                                /* dec is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isdigit(*t_ptr))
			{
				printf("Error: invalid decimal operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNum = atoi(pStr);
		if (lNeg)
			lNum = -lNum;

		return lNum;
	}
	else if (*pStr == 'x')        /* hex     */
	{
		pStr++;
		if (*pStr == '-')                                /* hex is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isxdigit(*t_ptr))
			{
				printf("Error: invalid hex operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
		lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
		if (lNeg)
			lNum = -lNum;
		return lNum;
	}
	else
	{
		printf("Error: invalid operand, %s\n", orig_pStr);
		exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
	}
}