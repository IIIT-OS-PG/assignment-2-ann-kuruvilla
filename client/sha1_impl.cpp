#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <openssl/sha.h>
#include "sha1_impl.h"
/***************************************g++ sha_impl256.cpp -lssl -lcrypto ***********/

using namespace std;

#define BLK_SIZE 512//-----------------------BLK_SIZE make to 512 kb specified in qn
//#define SHA_LENGTH 160
#define MAX_BLOCKS 20

const char* filehash(char filenm[100])
{
	FILE *f;
	int i;
	f=fopen(filenm,"rb");

	if(f==NULL)
	{
		perror("Error opening file\n");
		exit(1);
	}
	char buf[BLK_SIZE];
	char outputBuffer[70];
	size_t sz;
	//char s[500]="husaabms are  us vhdddddddddd dvhuuuuuuuuuuuuu hvuuuuuuuuuuuuuu vgdddddddddddd gveeeeeeeeeeeeeeeee vhdddddddddddddd";
	SHA_CTX md;
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Init(&md);

	int blk=0;
	// while((sz=fread((void*) buf,1,MAX_BLOCKS,f)>0)) // in one fread 20 blks each of size 1 b is read
	// {
		
	// 	SHA256_Update(&md,buf,sz);	
	// 	blk++;				//splits/reads in blocks of size 9 bytes (BLK_SIZE -1)
	// 	memset(buf,0,sizeof(buf));
	// }
	// SHA256_Final(hash,&md);

	//   i = 0;
 //    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
 //    {
 //        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
 //    }
	
	//  //sha256_hash_string(hash, outputBuffer);
	// cout<<outputBuffer<<endl;
	// cout<<"blkls"<<blk<<endl;


	/************************************************************************************************totl file hash****/

	/*****************************piece hash***********************************************/
	//memset(outputBuffer,0,sizeof(outputBuffer));
	//memset(hash,0,sizeof(hash));
	//fseek(f,0,SEEK_SET);
	string str="";
	while(!feof(f))
	{
		if((sz=fread((void*)buf,1,MAX_BLOCKS,f ))>0) //reads a piece of size maxblks in 1 read and finds its hash
		{
			SHA1_Init(&md);
			SHA1_Update(&md,buf,sz);	
			SHA1_Final(hash,&md);
			blk++;
			for(i = 0; i < SHA_DIGEST_LENGTH; i++)
	    	{
	        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);// hash in outputBuffer
	    	}
	    	//cout<<outputBuffer<<"\n";
	    	for(i=0;i<20;i++)
	    	str+=outputBuffer[i];
	    	memset(outputBuffer,0,sizeof(outputBuffer));
			memset(hash,0,sizeof(hash));
			

		}
	}
	cout<<endl<<str<<"hash of totl file"<<endl;

	fclose(f);
return str.c_str();	
}

// int main()
// {
// 	string s="1.txt";
// 	char* fi=(char*)s.c_str();
// 	filehash(fi);
// 	return 0;
// }