#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>


using namespace std;

#define queuelimit 5
#define BLK_SIZE 10

int main(int argc, char* argv[])
{
	char trackerip1[20];char trackerport2[20];
	char trackerport1[20]; char trackerip2[20];
	if(argc==1)
		cout<<"No extra command line arguments\n";
	struct sockaddr_in serveraddrss;
	struct sockaddr_in newaddrs;
	socklen_t addrlen;
	int newsock;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	if(socketfd < 0)	//tcp -sock_stream  af_inet - ipv4
	{
		perror("socket failed\n");
		exit(1);
	}
	/*--------------------------------OPENING tracker_info.txt------------------------------------*/
	FILE *fp;
	char buf[BLK_SIZE];
	size_t sz;
	char s[20];
	int size;
	size=20;
	fp=fopen(argv[1],"r");
	if(fp==NULL)
	{
		perror("Error opening track_info.txt\n");
		exit(1);
	}
	
	if(fgets(s, size, fp) !=NULL)
	{
		
		//trackerip1=s;
		strncpy(trackerip1,s,20);
		//s='\0';
		//cout<<trackerip1<<"\n";
	}
	if(fgets(s, size, fp) !=NULL)
	{
		
		strncpy(trackerport1,s,20);
		//cout<<trackerport1<<"\n";
	}
	if(fgets(s, size, fp) !=NULL)
	{
		
		strncpy(trackerip2,s,20);
		//cout<<trackerip2<<"\n";
	}
	if(fgets(s, size, fp) !=NULL)
	{
		
		strncpy(trackerport2,s,20);
		//cout<<trackerport2<<"\n";
	}
	fclose(fp);

	/************************************************************BIND*************************************************/
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
    serveraddrss.sin_addr.s_addr = inet_addr(trackerip1); 
    cout<<trackerip1<<"\n";
    unsigned short number = (unsigned short) strtoul(trackerport1, NULL, 0);
    //cout<<trackerport1<<"val of port1\n";
    //cout<<number<<"in tracker connecting to this port\n";
    serveraddrss.sin_port = htons( number );
    int retu = bind(socketfd,(struct sockaddr*) &serveraddrss, sizeof(serveraddrss)); 
    if(retu < 0)
    {
    	cout<<"Error in binding\n";
    	exit(1);
    }

    /****************************************************LISTEN************************************************************/
    int status = listen(socketfd,queuelimit);
    if(status < 0)
    {
    	cout<<"error in listen\n";
    	exit(1);
    }
    /********************************accept connection****************************************************************************/
    addrlen= sizeof(newaddrs);
    newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen); //accept is a blckng call
    char file_name[30];
    ssize_t reet= recv(newsock,file_name,sizeof(file_name),0);
    cout<<file_name<<"file name got\n";

    fp=fopen(file_name,"rb");
    if(fp==NULL)
    {
    	perror("error opening file");
    	//exit()
    }

    /*-----------return val of fread()-----This number equals the number of bytes transferred only when size is 1(1 byte).*/
    while(!feof(fp))
    {
    	sz=fread((void*)buf, BLK_SIZE-1, 1,fp);
    	//buf[9]='\0';
    	send(newsock,(void*)buf,strlen(buf),0);
    	memset(buf,0,sizeof(buf));
	}
    
    close(newsock);


    close(socketfd);
	return 0;
}