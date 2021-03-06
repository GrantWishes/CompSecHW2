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
	/* Figuring out encryption or decryption */
	if(strcmp(argv[1],"encrypt") == 0) {
		//printf("Encrypting!\n");
		inFile = fopen(argv[4],"rb");
		outFile = fopen(argv[3],"wb");
	} else if(strcmp(argv[1],"decrypt") == 0) {
		//printf("Decrypting!\n");
		inFile = fopen(argv[3],"rb");
		outFile = fopen(argv[4],"wb");
	} else {
		fprintf(stderr, "First argument must be 'encrypt' or 'decrypt'!\n");
		exit(1);
	}

	/* Error checking woo */
	if(keyFile == NULL || inFile == NULL || outFile == NULL) {
		fprintf(stderr, "Error opening file");
		exit(1);
	}

	int size;			// size of everything in bytes
	unsigned char keyBuffer;	// reading in key space
	unsigned char inBuffer;		// reading in input space
	unsigned char output;		// reading in output space
	fread(&size,sizeof(int),1, keyFile); // eat the data
	fread(&size,sizeof(int),1, inFile);  // get the same value as above (lol)
	fwrite(&size,sizeof(int),1, outFile);
	//printf("Size of file is %d\n",size);
	
	// Hit it with the one time pad
	for(int i = 0; i < size; i++) {

		fread(&keyBuffer, sizeof(unsigned char), 1, keyFile);
		fread(&inBuffer,  sizeof(unsigned char), 1, inFile);
		output = keyBuffer ^ inBuffer;
		fwrite(&output, sizeof(unsigned char), 1, outFile);
	}

	fclose(keyFile);
	fclose(inFile);
	fclose(outFile);
	return 0;
}
