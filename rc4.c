#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>

#define ENCRYPT (0)
#define DECRYPT (1)

int main(int argc, char* argv[]) {

	/* Just error checking */
	if(argc != 5) {
		printf("Not correct amount of arguments\n");
		exit(1);
	}

	FILE *keyFile;
	FILE *cipherFile;
	FILE *plainFile;

	keyFile = fopen(argv[2],"rb");

	/* Figuring out keysize and reading in key to key[] */
	int keySize;
	fread(&keySize,sizeof(int),1, keyFile);	
	unsigned char key[keySize];
	for(int k = 0; k < keySize; k++) {
		fread(&key[k], sizeof(unsigned char), 1, keyFile);
	}


	int inSize; // declaring input size here for some reason, don't ask
	int option; // what mode, encrypt or decrypt?
	unsigned char keyIV[32]; // will be key xor IV

	/* If we want to encrypt */
	if(strcmp(argv[1],"encrypt") == 0) {
		cipherFile = fopen(argv[3],"wb");
		plainFile = fopen(argv[4],"rb");
		
		fread(&inSize, sizeof(int), 1, plainFile);

		option = ENCRYPT;
	/* If we want to decrypt */
	} else if(strcmp(argv[1],"decrypt") == 0) {
		cipherFile = fopen(argv[3],"rb");
		plainFile = fopen(argv[4],"wb");

		fread(&inSize, sizeof(int), 1, cipherFile);

		option = DECRYPT;
	/* If we want the user to be smarter */
	} else {
		printf("Must be 'encrypt' or 'decrypt' for the first argument!\n");
		exit(1);
	}

	
	unsigned char IV[32];	// for the IV, duh
	unsigned char inBuffer; // input bytes
	unsigned char outBuffer;// output bytes
	unsigned char i;	// rc4 shenanigans
	unsigned char j;	// rc4 shenanigans
	unsigned char S[256];	// rc4 shenanigans
	unsigned char keystream;// what we xor with message
	unsigned char temp;	// for swaps

	switch(option) {
		case ENCRYPT:

			RAND_bytes(IV, 32); //error checking is for plebs		
	
			/* Get our key/IV super secret power combo */	
			for(int k = 0; k < 32; k++) {
				keyIV[k] = key[k] ^ IV[k];
			}

			int outSize;		// output size is + 32 for IV_SIZE
			outSize = 32 + inSize;	// but i ignore this anyway, don't ask		

			/* Write the size out */
			fwrite(&outSize, sizeof(int),1,cipherFile);
			
			/* Write the IV */
			fwrite(IV, 32, 1, cipherFile);

			/* Set up our permuation array */
			for(int k = 0; k < 256; k++) {
				S[k] = (unsigned char) k;
			}

			/* For some reason this algorithm breaks if key size > 48
 			* so I hope you weren't lying on the assignment when you said
 			* key size is between 16 and 32 bytes...
 			*/			
			j = 0;
			for(int k = 0; k < 256; k++) {
				j = (j + S[k] + keyIV[k % keySize]) % 256;
				temp = S[k];
				S[k] = S[(int)j];
				S[(int)j] = temp;
			}

			/* RC4 algorithm, see wiki for details */
			i = 0;
			j = 0;
			for(int k = 0; k < (3072 + inSize); k++) {
				i = (i + 1) % 256;
				j = (j + S[i]) % 256;
				
				temp = S[i];
				S[i] = S[j];
				S[j] = temp;
				
				/* Trash the first bit */
				if(k < 3072) {
					continue;
				}	

				keystream = (S[i] + S[j]) % 256;

				/* Read in what we want to encrypt (slowly) */
				fread(&inBuffer, sizeof(unsigned char), 1, plainFile);				

				/* ENCRYPTION */
				outBuffer = inBuffer ^ keystream;

				/* And write it to our file and keep going */  
				fwrite(&outBuffer,sizeof(unsigned char),1,cipherFile);	
			}
			break;
		case DECRYPT:

			/* Read in the IV */
			fread(IV, 32, 1, cipherFile);

			inSize = inSize - 32; // I don't need the IV size on here
			fwrite(&inSize, sizeof(int), 1, plainFile); // i didn't think this through really			
		
			/* From here to my next message, it's the same as ENCRYPT */
			for(int k = 0; k < 32; k++) {
				keyIV[k] = key[k] ^ IV[k];
			}

			for(int k = 0; k < 256; k++) {
				S[k] = (unsigned char) k;
			}

			j = 0;
			for(int k = 0; k < 256; k++) {
				j = (j + S[k] + keyIV[k % keySize]) % 256;
				temp = S[k];
				S[k] = S[(int)j];
				S[(int)j] = temp;
			}

			i = 0;
			j = 0;
				
			for(int k = 0; k < (3072 + inSize); k++) {
				i = (i + 1) % 256;
				j = (j + S[i]) % 256;
				
				temp = S[i];
				S[i] = S[j];
				S[j] = temp;
				
				if(k < 3072) {
					continue;
				}	

				keystream = (S[i] + S[j]) % 256;
		
				/* And just write bring it back to the original message */	
				fread(&inBuffer, sizeof(unsigned char), 1, cipherFile);
				outBuffer = inBuffer ^ keystream;
				fwrite(&outBuffer,sizeof(unsigned char),1, plainFile);
			}
			break;
		default:
			printf("You shouldn't be here!\n");
			break;
	}

	/* Cleanup, we did it! */
	fclose(keyFile);
	fclose(cipherFile);
	fclose(plainFile);
	
	return 0;
}
