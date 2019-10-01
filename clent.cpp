#include<iostream>
#include <unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

using namespace std;
int main()
{
	int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	int n;
	char Buffer [ BUFF_SIZE] ; 
	//char buffr[1024];
	if(clientsoc  < 0)
	{
		perror("client socket creation failed\n");
		exit(1);
	}
	

	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_addr.sin_port = htons( 2000 );
    int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect failed for client\n");
    	exit(1);
    }
/************************************RECEVING A WORD*************************************************/
    // recv(clientsoc,buffr,1024,0);

    // //cout<<"data received : "<<buffr<<"\n";
/********************************************************************************************/

    cout<<"Sending file from client to server\n";
    FILE* fp= fopen("/home/user/os_assignmnt2/2019201024_OS2/1.txt","rb");
    fseek(fp,0,SEEK_END); // ----------------moving pointer to the end
    int size=ftell(fp); //gets the size of file
    rewind(fp);
    send ( clientsoc, &size, sizeof(size), 0);
    
	while ( ( n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp ) ) > 0  && size > 0 )
	{
		cout<<n<<"snd\n";
		send (clientsoc, Buffer, n, 0 );
   	 	memset ( Buffer , '\0', BUFF_SIZE);
		size = size - n ;
	}

	fclose ( fp );
	close( clientsoc);
    return 0;



}