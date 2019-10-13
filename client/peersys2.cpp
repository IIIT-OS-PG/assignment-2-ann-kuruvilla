
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
#define buffer_size 200
#define BUFF_SIZE 10
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* service_reqclient(void* args)
{
	char in[100];
	char filenamee[buffer_size];
	char buffer[BUFF_SIZE];
	FILE* fp1;
	//cout<<"client enter command\n";
	cin>>in;
	cout<<in;
	int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	if(clientsoc  < 0)
	{
		perror("client socket creation failed\n");
		exit(1);
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	//unsigned short number = (unsigned short) strtoul(trackerport11, NULL, 0);
	//cout<<"Tracker port number "<<trackerport11<<endl;
	//cout<<number<<"\n";
    serv_addr.sin_port = htons(2000);

    int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect to server failed for client\n");
    	exit(1);
    }
    send(clientsoc,(void*)in,strlen(in),0); //---------------sending command name
    cout<<"send command name to tracker\n";

		cout<<"enter filename (no space in input)\n";
		cin>>filenamee;
		   //cout<<filenamee;
	    send(clientsoc,filenamee,buffer_size,0);
	    int file_size,n;

		 fp1=fopen("3.txt","wb");
	     if(fp1==NULL)
	     {
	        perror("Error opening file to write\n");
	       
	     }
	    memset ( buffer , '\0', BUFF_SIZE);
	   	n=recv(clientsoc,buffer,BUFF_SIZE,0); //------write as frst chunk
		cout<<"Received"<<n<<"bytes\n";
		cout<<buffer<<"\n";
		size_t rt=fwrite(buffer,sizeof(char),n,fp1);
		memset ( buffer , '\0', BUFF_SIZE);
		fclose(fp1);
close(clientsoc);
pthread_exit(NULL);
}

void* service_reqchildserv(void* argus)
{
	FILE *fp;
	fp=fopen("1.txt","wb");
	char buff[50]="ann kuuruvi";
	long i;
	i=(long)argus;
	cout<<i<<"thread\n";
	pthread_mutex_lock(&mutex);
		if(i==0)
		{
			fwrite(buff,sizeof(char),strlen(buff),fp);
		fclose(fp);
		}
		else
		{ 
			fseek(fp,0,SEEK_END);
		fwrite(buff,sizeof(char),strlen(buff),fp);
		fclose(fp);
		}
	pthread_mutex_unlock(&mutex);
	// int x=20;
	// int y=10;
	// int s;
	//int t=(int)argus;
	//cout<<t<<"th thread\n";
	//s=x+y;
	//cout<<s<<endl;
	pthread_exit(NULL);
}

void* service_reqserver(void *arg)
{
	cout<<"server\n";
	int i=0;
	int k;
	pthread_t tid3[10];
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
	cout<<"Socket created\n";
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
	serveraddrss.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddrss.sin_port = htons( 2078 );
	int retu = bind(socketfd,(struct sockaddr*) &serveraddrss, sizeof(serveraddrss)); 
    if(retu < 0)
    {
    	cout<<"Error in binding\n";
    	exit(1);
    }
    cout<<"Bind successful\n";
    int status = listen(socketfd,queuelimit);
    if(status < 0)
    {
    	cout<<"error in listen\n";
    	exit(1);
    }
    cout<<"Server Listening\n";
    addrlen= sizeof(newaddrs);

	while(newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen))
    {
	  pthread_create(&tid3[i++], NULL, service_reqchildserv, (void*)i);
	
	}
	for( k=0;k<i;k++)
     pthread_join(tid3[k],NULL);

pthread_exit(NULL);

}

int main(int argc, char* argv[])
{

	pthread_t tid1;
	pthread_t tid2;
	if(pthread_create(&tid1, NULL, service_reqserver,NULL )!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    }
	if(pthread_create(&tid2, NULL, service_reqclient, NULL)!= 0) 
	{
		cout<<"failed to create thread\n";
	}   

	pthread_join(tid1,NULL); 
	pthread_join(tid2,NULL); 
	
	pthread_exit(NULL);
		return 0;
}	