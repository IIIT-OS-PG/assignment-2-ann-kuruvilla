#include<iostream>
#include<stdio.h>
#include <fstream> 
#include<fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;
#define queuelimit 5
#define BUFF_SIZE 1024

int main()
{

	struct sockaddr_in serveraddrss;
    FILE* fp;
	int newsock,n;
    size_t rt;
	struct sockaddr_in newaddrs;
	socklen_t addrlen;
	char buffer[BUFF_SIZE];
    //string fi="/home/user/os_assignmnt2/2019201024_OS2/2.txt";
    //char* filenm= fi.c_str();
	
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	if(socketfd < 0)	//tcp -sock_stream  af_inet - ipv4
	{
		perror("socket failed\n");
		exit(1);
	}
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
    serveraddrss.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serveraddrss.sin_port = htons( 2000 );// connct client to this server addrs

    int retu = bind(socketfd,(struct sockaddr*) &serveraddrss, sizeof(serveraddrss)); 
    if(retu < 0)
    {
    	cout<<"Error in binding\n";
    	exit(1);
    }

    int status = listen(socketfd,queuelimit);
    if(status < 0)
    {
    	cout<<"error in listen\n";
    	exit(1);
    }
    
    /********************************accept connection****************************************************************************/
    addrlen= sizeof(newaddrs);
    newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen); //accept is a blckng call
    
    // strcpy(buffer,"hello");
    // send(newsock,buffer,strlen(buffer),0);

    fp= fopen("/home/user/os_assignmnt2/2019201024_OS2/2.txt","wb");
    if(fp==NULL)
    {
        perror("error opening file\n");
        exit(1);
    }
    cout<<"sending data to client\n";
    int file_size;
    ssize_t reet= recv(newsock,&file_size,sizeof(file_size),0);
    if(reet<0)
    {
        perror("Error in receiving\n");
        //exit(1);
    }

    while((n=recv(newsock,buffer,BUFF_SIZE,0))>0 && file_size>0)
    {
        cout<<n<<"receivd n byts at server\n";
        rt=fwrite(buffer,sizeof(char),n,fp);
        if(rt<0)
        {
            perror("write to file nt successfl\n");
            exit(1);
        }
        memset ( buffer , '\0', BUFF_SIZE);
        file_size = file_size - n;
    }

    close( socketfd);
//close( serverfd)
fclose ( fp );

  return 0;  

}