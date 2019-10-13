#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
#define buffer_size 200
#define BUFF_SIZE 10

int main(int argc, char* argv[])
{
	if(argc==1)
		cout<<"No extra command line arguments\n";
	char trackerip11[20];char trackerport21[20]; // --------storing port and ip of 2 trackers
	char trackerport11[20];char trackerip21[20];
	FILE* fp1;
	char s[20];
	char buffer[BUFF_SIZE];
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

	 
    //string commandn="abc.txt";

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
    //send(clientsoc,argv[3],sizeof(argv[3]),0);
     //const char* command=commandn.c_str();
    char command[200];
    char filenamee[buffer_size];
    //strcpy(command,"bac.txt");
    cout<<"waiting for command(no space)\n";
    cin>>command;
    cout<<command<<endl;
    int nu=send(clientsoc,(void*)command,strlen(command),0);
    if(nu==-1)
    {
    	cout<<"err";
    	perror("error in send()");
    	exit(1);
    }
   cout<<"send command name to tracker\n";

   cout<<"enter filename (no space in input)\n";
   cin>>filenamee;
   cout<<filenamee;
    send(clientsoc,filenamee,buffer_size,0);
    int file_size,n;
    recv(clientsoc,&file_size,sizeof(file_size),0);
    cout<<"Size of file requested: "<<file_size;
     fp1=fopen("3.txt","wb");
     if(fp1==NULL)
     {
        perror("error opening file\n");
        //exit(1);
     }
     while((n=recv(clientsoc,buffer,BUFF_SIZE,0))>0 && file_size>0)
     {
     	cout<<n<<"receivd "<<n<<" byts at client\n";
     	cout<<buffer<<"recevd txt\n";
     	 size_t rt=fwrite(buffer,sizeof(char),n,fp1);
     	if(rt<0)
        {
            perror("write to file nt successfl\n");
            exit(1);
        }
         memset ( buffer , '\0', BUFF_SIZE);
        file_size = file_size - n;
        n=0;
     }
     fclose(fp1);
    close(clientsoc);
	return 0;
}