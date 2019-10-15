#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;
#define queuelimit 30

void* service_reqclient(void* args)
{
	cout<<"client\n";
}
void* service_reqserver(void	*arg)
{
	pthread_t tid1;
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
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
	serveraddrss.sin_addr.s_addr = inet_addr(127.0.0.1);
	serveraddrss.sin_port = htons( 2356 );
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
    addrlen= sizeof(newaddrs);
    newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen);
    if(pthread_create(&tid3, NULL, service_reqchildserv, &newsock)!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    }
	pthread_join(tid3,NULL); 
	         
}

int main(int argc, char* argv[])
{
	if(argc==1)
		cout<<"No extra command line arguments\n";

	pthread_t tid1;
	pthread_t tid2;

	if(pthread_create(&tid1, NULL, service_reqserver,NULL )!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    }
	// if(pthread_create(&tid2, NULL, service_reqclient, NULL)!= 0) 
	// {
	// 	cout<<"failed to create thread\n";
	// }   

	    

	pthread_join(tid1,NULL); 
	//pthread_join(tid2,NULL); 

	return 0;

}