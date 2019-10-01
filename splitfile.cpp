#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <openssl/sha1>

using namespace std;

#define BLK_SIZE 10//-----------------------BLK_SIZE make to 512 kb specified in qn

void filehash()
{
	FILE *f;
	f=fopen("1.txt","rb");

	if(f==NULL)
	{
		perror("Error opening file\n");
		exit(1);
	}
	char buf[BLK_SIZE];
	size_t sz;
	SHA_CTX md;
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Init(&md);

	while(!feof(f))
	{
		sz=fread((void*) buf,9,1,f);
		SHA1_Update(&md,buf,strlen(buf));
		//cout<<buf;					//splits/reads in blocks of size 9 bytes 
		memset(buf,0,sizeof(buf));
	}
	HA1_Final(hash,&md);
	

	fclose(f);
	
}

int main()
{
	filehash();
	return 0;
}