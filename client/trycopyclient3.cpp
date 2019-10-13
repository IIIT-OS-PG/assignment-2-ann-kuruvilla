#include <bits/stdc++.h>
#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#include "sha1_impl.h"

using namespace std;


int main(int argc, char* argv[])
{
	if(argc==1)
		cout<<"No extra command line arguments\n";
	char trackerip11[20];char trackerport21[20]; // --------storing port and ip of 2 trackers
	char trackerport11[20];char trackerip21[20];
	FILE* fp1;
	char s[20];
	int size;
	size=20;
	fp1=fopen(argv[3],"r");
	if(fp1==NULL)
	{
		perror("Error opening track_info.txt\n");
		exit(1);
	}
	if(fgets(s, size, fp1) !=NULL)
	{
		
		strncpy(trackerip11,s,20);
	}
	if(fgets(s, size, fp1) !=NULL)
	{
		
		strncpy(trackerport11,s,20);
	}
	if(fgets(s, size, fp1) !=NULL)
	{
		
		strncpy(trackerip21,s,20);
		//cout<<trackerip2<<"\n";
	}
	if(fgets(s, size, fp1) !=NULL)
	{
		
		strncpy(trackerport21,s,20);
		//cout<<trackerport2<<"\n";
	}
	fclose(fp1);

	int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	if(clientsoc  < 0)
	{
		perror("client socket creation failed\n");
		exit(1);
	}

	 
    //string commandn;
	char command[200];
	char filenme[200];
	char hash[10000];
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr(trackerip11); 
	unsigned short number = (unsigned short) strtoul(trackerport11, NULL, 0);
	cout<<"Tracker port number "<<trackerport11<<endl;
	//cout<<number<<"\n";
    serv_addr.sin_port = htons(number);

    int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect to tracker1 failed for client\n");
    	exit(1);
    }
    

    send(clientsoc,argv[1],1000,0);   //client sendng ip and port to trckr

    send(clientsoc,argv[2],100,0);
  	 cin.getline(command,200);//-------------------------------upload_file abc.txt groupid //inpt format
     //cout<<command;
     //cout<<strlen(command);
     //send(clientsoc,command,strlen(command),0);
     
     
     char *w=strtok(command," ");
     //printf("%s\n",w );
     w=strtok(NULL," ");
     const char* h=filehash(w);
     strcpy(hash,h);
     //cout<<hash;
     strcpy(filenme,w);
     send(clientsoc,filenme,strlen(filenme),0);
     send(clientsoc,hash,strlen(hash),0);
     memset(&command,'\0',sizeof(command));
    close(clientsoc);
	return 0;
}