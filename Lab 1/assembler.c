#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

FILE* infile = NULL;
FILE* outfile = NULL;

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define INT_MAX       2147483647    /*maximum (signed) int value*/
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
	ORIG, END, FILL, NonLabel
};

enum {	/*Addressing modes*/
	REGISTER, IMMEDIATE, LABEL
};

int getOpcode(char*);
int toNum(char*);
int assemblyToDec(char*, char*, char*, char*, int, int);
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

	char lLine[255 + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;

	int tableIndex = 0;
	int pc = -1;
	int end = 0;
	int pass;
	for (pass = 1; pass <= 2; pass++)		/*Make 2 passes*/
	{
		do {
			lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
			if (lRet != DONE && lRet != EMPTY_LINE) {
				if (pass == 1) {				/*1st pass: Fill in the symbol table*/
					if (getOpcode(lOpcode) == ORIG) {
						if (pc != -1)
							exit(4);
						pc = toNum(lArg1);
						if (pc % 2 == 1)
							exit(3);
						if (pc < 0 || pc > 0xFFFF)
							exit(3);		/*Invalid constant (out of range)*/
						continue;
					}
					if (pc == -1)
						exit(4);		/*pc is never initialized so .ORIG is not the first thing.*/
					/*else if (getOpcode(lOpcode) == END) {
						break;
					}*/
					if (strcmp(lLabel, "") != 0) {
						int i;
						for (i = 0; i < tableIndex; i++)
						{
							if (strcmp(lLabel, symbolTable[i].label) == 0)
								exit(4);
						}
						if (getOpcode(lLabel) == NonLabel)
							exit(4);		/*Invalid label*/
						int j = 0;
						while (lLabel[j] != '\0') {
							if (isalnum(lLabel[j]) == 0)
								exit(4);
							j++;
						}
						symbolTable[tableIndex].address = pc;
						strcpy(symbolTable[tableIndex].label, lLabel);
						tableIndex++;
					}
				}
				else if (pass == 2) {
					if (getOpcode(lOpcode) == ORIG) {
						if (pc != -1)
							exit(4);
						pc = toNum(lArg1);
						if (pc % 2 == 1)
							exit(3);
						if (pc < 0 || pc > 0xFFFF)
							exit(3);		/*Invalid constant (out of range)*/
						continue;
					}
					if (pc == -1)
						exit(4);		/*pc is never initialized so .ORIG is not the first thing.*/
					else {
						int result = assemblyToDec(lOpcode, lArg1, lArg2, lArg3, tableIndex, pc);
						if (result == -1) {
							lRet = DONE;
							end++;
							break;
						}
						fprintf(outfile, "0x%.4X\n", result);
					}
				}
				if (pc > 0xFFFF)
					exit(4);		/*Program goes beyond memory*/
				pc = pc + 2;		/*Increment the PC*/
			}
		} while (lRet != DONE);
		int i;
		for (i = 0; i < tableIndex; i++)
		{
			printf("%d\t%s\n", symbolTable[i].address, symbolTable[i].label);
		}
		pc = -1;
		rewind(infile);

	}

	if (end == 0)
		exit(4);	/*There is no .END*/

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
	if (strcmp(line, "in") == 0 || strcmp(line, "out") == 0 || strcmp(line, "getc") == 0
		|| strcmp(line, "puts") == 0 || line[0] == 'x') 
		return NonLabel;
	if (line[2] == '\0' && line[0] == 'r') {
		int regNum = line[1] - 0x30;
		if (regNum < 8)
			return NonLabel;
	}
	if (line[0] - 0x30 >= 0 && line[0] - 0x30 <= 9)
		return NonLabel;
	return -1;
}

int registerNumber(char* arg) {		/*Must check that it's a register first!*/
	return arg[1] - 0x30;
}

int findLabel(char* label, int tableIndex) {
	int labelAddress = -1;
	int i;
	for (i = 0; i < tableIndex; i++)
	{
		if (strcmp(symbolTable[i].label, label) == 0) {
			labelAddress = symbolTable[i].address;
			break;
		}
	}
	if (labelAddress == -1)
		exit(1);		/*Lable doesn't exist*/
	return labelAddress;
}

int assemblyToDec(char * pOpcode, char * pArg1, char * pArg2, char * pArg3, int tableIndex, int pc) {
	int addressingMode = -1;
	int opcode = getOpcode(pOpcode);
	if (opcode == -1 || opcode == NonLabel)
		exit(2);
	if (opcode == ADD) {
		addressingMode = checkAddressingMode(pArg3);
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (addressingMode == REGISTER) {
			return registerNumber(pArg3) + 64 * registerNumber(pArg2) + 512 * registerNumber(pArg1) + 4096;
		}
		else if (addressingMode == IMMEDIATE) {
			int value = 0;
			if (toNum(pArg3) < 0) {
				value = 32 + toNum(pArg3);
				if (value < 0 || value > 31)
					exit(3);
			}
			else {
				value = toNum(pArg3);
				if (value < 0 || value > 15)
					exit(3);
			}
			return value + 32 + 64 * registerNumber(pArg2) + 512 * registerNumber(pArg1) + 4096;
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
			return registerNumber(pArg3) + 64 * registerNumber(pArg2) + 512 * registerNumber(pArg1) + 20480;
		}
		else if (addressingMode == IMMEDIATE) {
			int value = 0;
			if (toNum(pArg3) < 0) {
				value = 32 + toNum(pArg3);
				if (value < 0 || value > 31)
					exit(3);
			}
			else {
				value = toNum(pArg3);
				if (value < 0 || value > 15)
					exit(3);
			}
			return value + 32 + 64 * registerNumber(pArg2) + 512 * registerNumber(pArg1) + 20480;
		}
		else
			exit(4);
	}
	if (opcode == BR) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			/*Address of label is PC+2 + (sign extended offset x 2) */
			/*PC Offset = (Label Address - (PC+2))/2 */
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset;
		}
		else
			exit(4);
	}
	if (opcode == BRn) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0? pcOffset*-1:pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 2048;
		}
		else
			exit(4);
	}
	if (opcode == BRz) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 1024;
		}
		else
			exit(4);
	}
	if (opcode == BRp) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 512;
		}
		else
			exit(4);
	}
	if (opcode == BRnz) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 2048 + 1024;
		}
		else
			exit(4);
	}
	if (opcode == BRzp) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 1024 + 512;
		}
		else
			exit(4);
	}
	if (opcode == BRnp) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 2048 + 512;
		}
		else
			exit(4);
	}
	if (opcode == BRnzp) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + 2048 + 1024 + 512;
		}
		else
			exit(4);
	}
	if (opcode == HALT) {
		if (strcmp(pArg1, "") != 0 || strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		return 0xF025;
	}
	if (opcode == JMP) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == REGISTER) {
			return registerNumber(pArg1) * 64 + 49152;
		}
		else exit(4);
	}
	if (opcode == JSR) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == -1)
			exit(1);				/*Invalid label*/
		if (checkAddressingMode(pArg1) == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg1, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 2047)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 2048 + pcOffset;
			return pcOffset + 2048 + 16384;
		}
		else exit(4);
	}
	if (opcode == JSRR) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == REGISTER) {
			return registerNumber(pArg1) * 64 + 16384;
		}
		else exit(4);
	}
	if (opcode == LDB) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {
			int offset = 0;
			if (toNum(pArg3) < 0) {
				offset = 64 + toNum(pArg3);
				if (offset < 0 || offset > 63)
					exit(3);
			}
			else {
				offset = toNum(pArg3);
				if (offset < 0 || offset > 31)
					exit(3);
			}
			return offset + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 8192;
		}
		else exit(4);
	}
	if (opcode == LDW) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {
			int offset = 0;
			if (toNum(pArg3) < 0) {
				offset = 64 + toNum(pArg3);
				if (offset < 0 || offset > 63)
					exit(3);
			}
			else {
				offset = toNum(pArg3);
				if (offset < 0 || offset > 31)
					exit(3);
			}
			return offset + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 24576;
		}
	}
	if (opcode == LEA) {
		if (strcmp(pArg3, "") != 0 || checkAddressingMode(pArg1) != REGISTER)
			exit(4);
		if (checkAddressingMode(pArg2) == -1)
			exit(1);				/*Invalid label*/
		if (addressingMode == LABEL) {
			int pcOffset = 0;
			int labelAddress = findLabel(pArg2, tableIndex);
			pcOffset = (labelAddress - (pc + 2)) / 2;
			int check = pcOffset<0 ? pcOffset*-1 : pcOffset;
			if (check < 0 || check > 511)
				exit(3);
			if (pcOffset < 0)
				pcOffset = 512 + pcOffset;
			return pcOffset + registerNumber(pArg1) * 512 + 57344;
		}
	}
	if (opcode == NOP) {
		if (strcmp(pArg1, "") != 0 || strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		return 0x0000;
	}
	if (opcode == NOT) {
		if (strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == REGISTER && checkAddressingMode(pArg2) == REGISTER) {
			return 63 + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 36864;
		}
	}
	if (opcode == RET) {
		if (strcmp(pArg1, "") != 0 || strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		return 0xC1C0;
	}
	if (opcode == LSHF) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {
			int value = toNum(pArg3);
			if (value < 0 || value > 15)
				exit(3);
			return value + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 53248;
		}
	}
	if (opcode == RSHFL) {
		int value = toNum(pArg3);
		if (value < 0 || value > 15)
			exit(3);
		return value + 16 + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 53248;
	}
	if (opcode == RSHFA) {
		int value = toNum(pArg3);
		if (value < 0 || value > 15)
			exit(3);
		return value + 16 + 32 + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 53248;
	}
	if (opcode == RTI) {
		if (strcmp(pArg1, "") != 0 || strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		return 0x8000;
	}
	if (opcode == STB) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {
			int offset = 0;
			if (toNum(pArg3) < 0) {
				offset = 64 + toNum(pArg3);
				if (offset < 0 || offset > 63)
					exit(3);
			}
			else {
				offset = toNum(pArg3);
				if (offset < 0 || offset > 31)
					exit(3);
			}
			return offset + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 12288;
		}
		else exit(4);
	}
	if (opcode == STW) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == IMMEDIATE) {
			int offset = 0;
			if (toNum(pArg3) < 0) {
				offset = 64 + toNum(pArg3);
				if (offset < 0 || offset > 63)
					exit(3);
			}
			else {
				offset = toNum(pArg3);
				if (offset < 0 || offset > 31)
					exit(3);
			}
			return offset + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 28672;
		}
		else exit(4);
	}
	if (opcode == TRAP) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		if (checkAddressingMode(pArg1) == IMMEDIATE) {
			if (pArg1[0] != 'x') {
				exit(4);		/*Given vector is not in hex*/
			}
			int value = 0;
			value = toNum(pArg1);
			if (value < 0 || value > 0xFF)
				exit(3);
			/*if (value > 0x25 && value <= 0xFF)
				exit(4);*/
			return value + 0xF000;
		}
	}
	if (opcode == XOR) {
		if (checkAddressingMode(pArg1) != REGISTER || checkAddressingMode(pArg2) != REGISTER) {
			exit(4);
		}
		if (checkAddressingMode(pArg3) == REGISTER) {
			return registerNumber(pArg3) + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 0x9000;
		}
		else if (checkAddressingMode(pArg3) == IMMEDIATE) {
			int value = 0;
			if (toNum(pArg3) < 0) {
				value = 32 + toNum(pArg3);
				if (value < 0 || value > 31)
					exit(3);
			}
			else {
				value = toNum(pArg3);
				if (value < 0 || value > 15)
					exit(3);
			}
			return value + 32 + registerNumber(pArg2) * 64 + registerNumber(pArg1) * 512 + 0x9000;
		}
	}
	if (opcode == FILL) {
		if (strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		int value = toNum(pArg1);
		int check = value < 0 ? value * -1 : value;
		if (check < 0 || check > 32767)
			exit(3);
		if (value < 0)
			value = 65536 + value;
		return value;
	}
	if (opcode == END) {
		if (strcmp(pArg1, "") != 0 || strcmp(pArg2, "") != 0 || strcmp(pArg3, "") != 0)
			exit(4);
		return -1;
	}
	exit(4);
}

int checkAddressingMode(char* arg) {
	if (arg[0] == '\0')
		exit(4);		/*The argument doesn't contain a value*/
	if (arg[0] == 'r' && (arg[1] - 0x30) >= 0 && (arg[1] - 0x30) <= 7 && arg[2] == '\0')
		return REGISTER;
	int labels = -1;
	int j = 0;
	while(strcmp(symbolTable[j].label, "") != 0)
	{
		if (strcmp(arg, symbolTable[j].label) == 0)
			return LABEL;
		j++;
	}
	int c = 0;
	while (arg[c] != '\0') {
		if (arg[0] != '#' && arg[0] != 'x') {
			return -1;
		}
		else if (c > 0) {
			if (arg[0] == '#') {
				if (arg[c] - 0x30 < 0 || arg[c] - 0x30 > 9) {
					if (arg[1] == '-') {
						c++;
						continue;
					}
					return -1;
				}
			}
			else if (arg[0] == 'x') {
				if (arg[c] - 0x30 < 0 || (arg[c] - 0x30 > 9 && arg[c] < 'a') || arg[c] > 'f') {
					if (arg[1] == '-') {
						c++;
						continue;
					}
					return -1;
				}
			}
		}
		c++;
	}
	/*int i = toNum(arg);*/
	return IMMEDIATE;
	exit(4);		/*The operand is not supported*/
}

int labelToAddress(char* label, int tableLength) {
	int i;
	for (i = 0; i < tableLength; i++)
	{
		if (strcmp(label, symbolTable[i].label) == 0)
			return symbolTable[i].address;
	}
	exit(1);			/*Lable undefined. Return Error code 1*/
}

int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
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

	if (getOpcode(lPtr) == -1 || getOpcode(lPtr) == NonLabel) /* found a label */
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

	*pArg4 = lPtr;

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
