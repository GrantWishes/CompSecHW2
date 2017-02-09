#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>

#define ENCRYPT (0)
#define DECRYPT (1)

int main(int argc, char* argv[]) {

	if(argc != 5) {
		printf("Not correct amount of arguments\n");
		exit(1);
	}

	FILE *keyFile;
	FILE *cipherFile;
	FILE *plainFile;

	keyFile = fopen(argv[2],"rb");

	int keySize;
	fread(&keySize,sizeof(int),1, keyFile);
	
	unsigned char key[keySize];
	fread(&key, keySize, 1, keyFile);

	int inSize;
	

	int option;
	unsigned char keyIV[32];

	if(strcmp(argv[1],"encrypt") == 0) {
		cipherFile = fopen(argv[3],"wb");
		plainFile = fopen(argv[4],"rb");
		
		fread(&inSize, sizeof(int), 1, plainFile);

		option = ENCRYPT;
	} else if(strcmp(argv[1],"decrypt") == 0) {
		cipherFile = fopen(argv[3],"rb");
		plainFile = fopen(argv[4],"wb");

		fread(&inSize, sizeof(int), 1, cipherFile);

		option = DECRYPT;
	} else {
		printf("Must be 'encrypt' or 'decrypt' for the first argument!\n");
		exit(1);
	}
	
	switch(option) {
		case ENCRYPT:
			printf("Encrypting!\n");

			

			int temp;
			unsigned char IV[32];
			RAND_bytes(IV, 32); //error checking is for plebs		
			*keyIV = *key ^ *IV; // if something is wrong, it is probably here

			int outSize;
			outSize = keySize + inSize;			

			fwrite(&outSize, sizeof(int),1,cipherFile);
			printf("Size of file is %d\n", outSize);

			fwrite(IV, 32, 1, cipherFile);

			unsigned char S[256];
			for(int i = 0; i < 256; i++) {
				S[i] = (unsigned char) i;
			}
			unsigned char j = 0;
			for(int i = 0; i < 256; i++) {
				j = (j + S[i] + keyIV[i % keySize]) % 256;
				temp = S[i];
				S[i] = S[j];
				S[j] = temp;
			}

			unsigned char i = 0;
			j = 0;
				
			for(int k = 0; k < (3072 + inSize); k++) {
				i = (i + 1) % 256;
				j = (j + S[i]) % 256;
				
				temp = S[i];
				S[i] = S[j];
				S[j] = temp;
				
			//	printf("%d\n", k);
				
				if(k < 3072) {
					continue;
				}	

				unsigned char keystream = (S[i] + S[j]) % 256;
			//	printf("%u\n", test);
			//	printf("%d\n",test);

				unsigned char output;
				output = keyIV[k-3072] ^ keystream;  
				fwrite(&output,sizeof(unsigned char),1,cipherFile);	
			}
			break;
		case DECRYPT:
			printf("Decrypting!\n");
			break;
		default:
			printf("You shouldn't be here!\n");
			break;
	}


	fclose(keyFile);
	fclose(cipherFile);
	fclose(plainFile);

	return 0;
}
