#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>


#define BLK_SIZE 10

using namespace std;

int main(int argc, char* argv[])
{

	char trackerip11[20];char trackerport21[20]; // --------storing port and ip of 2 trackers
	char trackerport11[20];char trackerip21[20];
	if(argc==1)
		cout<<"No extra command line arguments\n";
	/*-------------------------------------------------OPENING tracker_info.txt------------------------------------*/
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
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr(trackerip11); 
	unsigned short number = (unsigned short) strtoul(trackerport11, NULL, 0);
	cout<<trackerport11<<endl;
	cout<<number<<"\n";
    serv_addr.sin_port = htons(number);

    int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect to tracker1 failed for client\n");
    	exit(1);
    }
    string fn="1.txt";  //asking for this file from server
    int n;
    const char* filenm=fn.c_str();
    send(clientsoc,filenm,sizeof(filenm),0);
    char buff[BLK_SIZE];
    fp1=fopen("3.txt","wb");
    while(n=recv(clientsoc,buff,BLK_SIZE-1,0)>0)
    {
    	size_t rt=fwrite(buff,BLK_SIZE-1,1,fp1);
    	memset(buff,'\0',BLK_SIZE);
    	if(rt<0)
    	{
    		perror("error writing to file\n");
    	}
    }

    
    close(clientsoc);


return 0;
}
