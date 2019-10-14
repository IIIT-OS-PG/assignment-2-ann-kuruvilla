#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFF_SIZE 50
#define queuelimit 30
#define CHUNK_SIZE 512

using namespace std;

vector<string> splitbydelimeter(string s,char deli)
{
	int i;
	vector<string> v;
	string st="";
	for(i=0;i<s.size();i++)
	{
		if(s[i]!=deli)
		{
			st+=s[i];
		}
		else
		{
			v.push_back(st);
			st="";
		}

	}
	v.push_back(st);
	st="";

return v;	
}
void* service_reqserv(void* argus)
{
	char str[27]="127.0.0.1:3058=C1,C2,C4,C6";
	int filsize;
	//char strn[50];
	//strcpy(strn,str);

	int newsock=*((int *)argus);
	if(send(newsock,str,strlen(str),0))
	{
		perror("Send");
	}
	cout<<"Server sending file size\n";
	FILE *fp=fopen("1.txt","rb");
	fseek(fp,0,SEEK_END);
	filsize=ftell(fp);
	rewind(fp);
	send ( newsock, &filsize, sizeof(filsize), 0);
	//send(newsock,)
	char chunkidd[2];
	char buffer[CHUNK_SIZE];
	int n,chunkpos;
	while(recv(newsock,chunkidd,2,0))
	{
		cout<<chunkidd<<endl;
		fflush(stdout);
		chunkpos= chunkidd[1] - '0';//---------------integr of chunk number
		memset(chunkidd,0,2);
		fseek(fp,(chunkpos-1)*CHUNK_SIZE,SEEK_SET);
		n=fread(buffer,sizeof(char),CHUNK_SIZE,fp);
		send(newsock,buffer,n,0);
		memset(buffer,0,CHUNK_SIZE);
		n=0;

	}


close(newsock);
}

void* write_chunks(void* arggs)
{

	
}

/*---------------------------------P1 Peer System-------------------------------*/
void* service_reqclient(void* args)
{
	char command[100];
	cin>>command;

	int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	if(clientsoc  < 0)
	{
		perror("Client socket creation failed\n");
		exit(1);
	}
	char chunkstring[BUFF_SIZE];
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	serv_addr.sin_port = htons(2000);
	int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect to server failed for client\n");
    	exit(1);
    }


    recv(clientsoc,chunkstring,BUFF_SIZE,0);
    cout<<chunkstring;


close(clientsoc);
pthread_exit(NULL);
  }

int main(int argc, char* argv[])
{

	/*****************CLIENT THREAD**************************************************************/
	pthread_t tid1;
	if(pthread_create(&tid1, NULL, service_reqclient, NULL)!= 0) 
	{
		cout<<"Failed to create client thread\n";
	} 
	
	/*******************-----------------------------------SERVER--------------------------------****************/

	pthread_t tid2;
	struct sockaddr_in serveraddrss;
	struct sockaddr_in newaddrs;
	socklen_t addrlen;
	int newsock,i;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	
	// vector<string> parts;

	// parts=splitbydelimeter(str,'=');
	// string ip_port_pair=parts[0];
	// parts2 = splitbydelimeter(parts[1],',');//----------------parts2 contain C1,C2,C3,C4(chunk ids)




	if(socketfd < 0)	//tcp -sock_stream  af_inet - ipv4
	{
		perror("socket failed\n");
		exit(1);
	}
	cout<<"Socket created\n";
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
	serveraddrss.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddrss.sin_port = htons( 3625 );
	int retu = bind(socketfd,(struct sockaddr*) &serveraddrss, sizeof(serveraddrss)); 
    if(retu < 0)
    {
    	cout<<"Error in binding\n";
    	exit(1);
    }
    cout<<"Bind Successful\n";
    int status = listen(socketfd,queuelimit);
    if(status < 0)
    {
    	cout<<"error in listen\n";
    	exit(1);
    }
    addrlen= sizeof(newaddrs);

    if(newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen))
    {
	    //send(newsock,str,strlen(str),0);
	    if(pthread_create(&tid2,NULL,service_reqserv,&newsock)!=0)
	    {
	    	cout<<"Failed to create server request service thread\n";
	    }
	}

	pthread_exit(NULL);
    

	return 0;

}		