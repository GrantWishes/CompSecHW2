#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

	if(argc != 5) {
		fprintf(stderr, "Incorrect amount of arguments\n");
		exit(1);
	}

	FILE *keyFile;
	FILE *inFile;
	FILE *outFile;

	keyFile = fopen(argv[2],"rb");

	if(strcmp(argv[1],"encrypt") == 0) {
		printf("Encrypting!\n");
		inFile = fopen(argv[4],"rb");
		outFile = fopen(argv[3],"wb");
	} else if(strcmp(argv[1],"decrypt") == 0) {
		printf("Decrypting!\n");
		inFile = fopen(argv[3],"rb");
		outFile = fopen(argv[4],"wb");
	} else {
		fprintf(stderr, "First argument must be 'encrypt' or 'decrypt'!\n");
		exit(1);
	}
	
	fclose(keyFile);
	fclose(inFile);
	fclose(outFile);
	return 0;
}
