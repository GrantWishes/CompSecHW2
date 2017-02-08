#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main (int argc, char *argv[])
{

	if(argc != 3){
		printf("Missing command line arguments\n");
		return 0 ; 
	}

  FILE * fmsg = fopen(argv[1], "wb"); 
  FILE * fkey = fopen(argv[2], "wb");

  if(fmsg == NULL){
  	printf("Cannot open output file %s\n",argv[1]);
  	return 0 ; 
  }

  if(fkey == NULL){
    printf("Cannot open output file %s\n",argv[2]);
    return 0 ; 
  }

  /* initialize random seed: */
  srand (time(NULL));
  
  int len = rand() % 500 + 50;
  fwrite(&len, sizeof len, 1, fmsg); 
  fwrite(&len, sizeof len, 1, fkey);
  printf("Message size: %d\n",len); 
  int i ; 
  for(i=0;i<len;++i){
    unsigned char c = (unsigned char) (rand()%256);
    char a = c;
    int k;
    for (k = 0; k < 8; k++) 
    {
      printf("%d", !!((a << k) & 0x80));
    }
    printf(" ");

    fwrite(&c, sizeof c, 1, fmsg); 
  }
  printf("\nKey size: %d\n",len);
  int j ; 
  for(j=0;j<len;++j){
    unsigned char c_key = (unsigned char) (rand()%256);
    char a_key = c_key;
    int l;
    for (l = 0; l < 8; l++) 
    {
      printf("%d", !!((a_key << l) & 0x80));
    }
    printf(" ");

    fwrite(&c_key, sizeof c_key, 1, fkey); 
  }
  printf("\n");
  
  fclose(fmsg); 
  fclose(fkey);
  return 0;
}
