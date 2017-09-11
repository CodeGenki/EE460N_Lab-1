/*
Name 1: Ashkan Vafaee
Name 2: Genki Oji
UTEID 1: av28837
UTEID 2: gto99
*/

#define _CRT_SECURE_NO_WARNINGS		/* Remove this Later */
#include <stdio.h>	/* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h>	/* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */





int main(int argc, char* argv[]) {

	/* Acquires Input and Output file names */
	char *prgName = NULL;
	char *iFileName = NULL;
	char *oFileName = NULL;

	prgName = argv[0];
	iFileName = argv[1];
	oFileName = argv[2];

	printf("program name = '%s'\n", prgName);
	printf("input file name = '%s'\n", iFileName);
	printf("output file name = '%s'\n", oFileName);



	
	/* open the source file */
	FILE* infile = NULL;
	FILE* outfile = NULL;

	

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


	/* Do stuff with files */

	fclose(infile);
	fclose(outfile);
	





}