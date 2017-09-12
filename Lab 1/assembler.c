#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE* infile = NULL;
FILE* outfile = NULL;

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

enum {
	DONE, OK, EMPTY_LINE
};

enum {
	ADD, AND, BR, BRn, BRz, BRp, BRzp, BRnp, BRnz, BRnzp,
	HALT, JMP, JSR, JSRR, LDB, LDW, LEA, NOP, NOT, RET, LSHF,
	RSHFL, RSHFA, RTI, STB, STW, TRAP, XOR,
	ORIG, END, FILL
};

enum {	/*Addressing modes*/
	REGISTER, IMMEDIATE, LABEL
};

int getOpcode(char*);
int toNum(char*);
int assemblyToHex(char**, char**, char**, char**);
int checkAddressingMode(char*);

int main(int argc, char* argv[]) {

	char *prgName	= NULL;
	char *iFileName = NULL;
	char *oFileName = NULL;

	prgName		= argv[0];
	iFileName	= argv[1];
	oFileName	= argv[2];
	
	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");

	if (!infile) {
		printf("Error: Cannot open %s\n", argv[1]);
		exit(4);
	}
	if (!outfile) {
		printf("Error: Cannot open %s\n", argv[2]);
		exit(4);
	}

	char lLine[255 + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3;
	int lRet;

	for (int pass = 1; pass <= 2; pass++)		/*Make 2 passes*/
	{
		int pc;
		int tableIndex = 0;
		do {
			lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3);
			if (lRet != DONE && lRet != EMPTY_LINE) {
				if (pass == 1) {				/*1st pass: Fill in the symbol table*/
					if (getOpcode(lOpcode) == ORIG) {
						pc = toNum(lArg1);
						continue;
					}
					else if (getOpcode(lOpcode) == END) {
						break;
					}
					if (strcmp(lLabel, "") != 0) {
						symbolTable[tableIndex].address = pc;
						strcpy(symbolTable[tableIndex].label, lLabel);
						tableIndex++;
					}
				}
				else if (pass == 2) {
					assemblyToHex(lOpcode, lArg1, lArg2, lArg3);
				}
				pc = pc + 2;		/*Increment the PC*/
			}
		} while (lRet != DONE);
		for (int i = 0; i < tableIndex; i++)
		{
			printf("%d\t%s\n", symbolTable[i].address, symbolTable[i].label);
		}
	}
	
	fclose(infile);
	fclose(outfile);

}

int getOpcode(char* line) {	
	if (strcmp(line, "add") == 0)	return ADD;
	if (strcmp(line, "and") == 0)	return AND;
	if (strcmp(line, "br") == 0)	return BR;
	if (strcmp(line, "brn") == 0)	return BRn;
	if (strcmp(line, "brz") == 0)	return BRz;
	if (strcmp(line, "brp") == 0)	return BRp;
	if (strcmp(line, "brzp") == 0)	return BRzp;
	if (strcmp(line, "brnp") == 0)	return BRnp;
	if (strcmp(line, "brnz") == 0)	return BRnz;
	if (strcmp(line, "brnzp") == 0)	return BRnzp;
	if (strcmp(line, "halt") == 0)	return HALT;
	if (strcmp(line, "jmp") == 0)	return JMP;
	if (strcmp(line, "jsr") == 0)	return JSR;
	if (strcmp(line, "jsrr") == 0)	return JSRR;
	if (strcmp(line, "ldb") == 0)	return LDB;
	if (strcmp(line, "ldw") == 0)	return LDW;
	if (strcmp(line, "lea") == 0)	return LEA;
	if (strcmp(line, "nop") == 0)	return NOP;
	if (strcmp(line, "not") == 0)	return NOT;
	if (strcmp(line, "ret") == 0)	return RET;
	if (strcmp(line, "lshf") == 0)	return LSHF;
	if (strcmp(line, "rshfl") == 0)	return RSHFL;
	if (strcmp(line, "rshfa") == 0)	return RSHFA;
	if (strcmp(line, "rti") == 0)	return RTI;
	if (strcmp(line, "stb") == 0)	return STB;
	if (strcmp(line, "stw") == 0)	return STW;
	if (strcmp(line, "trap") == 0)	return TRAP;
	if (strcmp(line, "xor") == 0)	return XOR;
	if (strcmp(line, ".orig") == 0)	return ORIG;
	if (strcmp(line, ".end") == 0)	return END;
	if (strcmp(line, ".fill") == 0)	return FILL;

	return -1;
}

int assemblyToHex(char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3) {
	int addressingMode = -1;
	int opcode = getOpcode(pOpcode);
	if (opcode == ADD) {
		addressingMode = checkAddressingMode(pArg3);
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (addressingMode == REGISTER) {

		}
		else if (addressingMode == IMMEDIATE) {

		}
		else
			exit(4);
	}
	if (opcode == AND) {
		addressingMode = checkAddressingMode(pArg3);
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (addressingMode == REGISTER) {

		}
		else if (addressingMode == IMMEDIATE) {

		}
		else
			exit(4);
	}
	if (opcode == BR) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {
			/*Address of label is PC+2 + (sign extended offset x 2) */
			/*PC Offset = (Label Address - (PC+2))/2 */
		}
		else
			exit(4);
	}
	if (opcode == BRn) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == BRz) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == BRp) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == BRnz) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == BRzp) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == BRnp) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == BRnzp) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else
			exit(4);
	}
	if (opcode == HALT) {
		if (strcmp(pArg1, '\0') != 0 || strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		return 0xF025;
	}
	if (opcode == JMP) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == REGISTER) {

		}
		else exit(4);
	}
	if (opcode == JSR) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == LABEL) {

		}
		else exit(4);
	}
	if (opcode == JSRR) {
		if (strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == REGISTER) {

		}
		else exit(4);
	}
	if (opcode == LDB) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {

		}
		else exit(4);
	}
	if (opcode == LDW) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {

		}
	}
	if (opcode == LEA) {
		if (strcmp(pArg3, '\0') != 0 || checkAddressingMode(pArg1) != REGISTER)
			exit(4);
		if (checkAddressingMode(pArg2) == LABEL) {

		}
	}
	if (opcode == NOP) {
		if (strcmp(pArg1, '\0') != 0 || strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		return 0x0000;
	}
	if (opcode == NOT) {
		if (strcmp(pArg3, '\0') != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == REGISTER && checkAddressingMode(pArg2) == REGISTER) {

		}
	}
	if (opcode == RET) {
		if (strcmp(pArg1, '\0') != 0 || strcmp(pArg2, '\0') != 0 || strcmp(pArg3, '\0') != 0)
			exit(4);
		return 0xC1C0;
	}
	if (opcode == LSHF) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {

		}
	}
	if (opcode == RSHFL) {
		
	}
}

int checkAddressingMode(char* arg) {
	if (arg[0] == '\0')
		exit(4);		/*The argument doesn't contain a value*/
	if (arg[0] == 'R' && (arg[1] - 0x30) >= 0 && (arg[1] - 0x30) <= 7 && arg[2] == '\0')
		return REGISTER;
	int i = 0;
	int isNumber = 1;
	while (arg[i] != '\0') {
		if (arg[i] - 0x30 < 0 && arg[i] - 0x30 > 9) {
			isNumber = -1;
			break;
		}
		else isNumber = 1;
		i++;
	}
	if (isNumber == 1)
		return IMMEDIATE;
	int j = 0;
	while(symbolTable[j].address != 0)
	{
		if (strcmp(arg, symbolTable[j].label) == 0)
			return LABEL;
		j++;
	}
	exit(4);		/*The operand is not supported*/
}

int labelToAddress(char* label, int tableLength) {
	for (int i = 0; i < tableLength; i++)
	{
		if (strcmp(label, symbolTable[i].label) == 0)
			return symbolTable[i].address;
	}
	exit(1);			/*Lable undefined. Return Error code 1*/
}

int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3)
{
	char * lRet, *lPtr;
	int i;
	if (!fgets(pLine, 255, pInfile))
		return(DONE);
	for (i = 0; i < strlen(pLine); i++)
		pLine[i] = tolower(pLine[i]);

	/* convert entire line to lowercase */
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = pLine + strlen(pLine);

	/* ignore the comments */
	lPtr = pLine;

	while (*lPtr != ';' && *lPtr != '\0' && *lPtr != '\n')
		lPtr++;

	*lPtr = '\0';
	if (!(lPtr = strtok(pLine, "\t\n ,")))
		return(EMPTY_LINE);

	if (getOpcode(lPtr) == -1 && lPtr[0] != '.') /* found a label */
	{
		*pLabel = lPtr;
		if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	}

	*pOpcode = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg1 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg2 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	*pArg3 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	/* *pArg4 = lPtr; */

	return(OK);
}

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
