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

void* service_reqclient(void* args)
{
	//string in;
	cout<<"Client Thread\n";
	//cin>>in;
	// char in[100];
	// char filenamee[buffer_size];
	// char buffer[BUFF_SIZE];
	// FILE* fp1;
	// //cout<<"client enter command\n";
	// cin>>in;
	// cout<<in;
	// int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	// if(clientsoc  < 0)
	// {
	// 	perror("Client socket creation failed\n");
	// 	exit(1);
	// }
	// struct sockaddr_in serv_addr;
	// memset(&serv_addr, '\0', sizeof(serv_addr));
	// serv_addr.sin_family = AF_INET; 
	// serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	// //unsigned short number = (unsigned short) strtoul(trackerport11, NULL, 0);
	// //cout<<"Tracker port number "<<trackerport11<<endl;
	// //cout<<number<<"\n";
 //    serv_addr.sin_port = htons(2000);

 //    int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
 //    if(re<0)
 //    {
 //    	perror("connect to server failed for client\n");
 //    	exit(1);
 //    }
 //    send(clientsoc,(void*)in,strlen(in),0); //---------------sending command name
 //    cout<<"send command name to tracker\n";

	// cout<<"enter filename (no space in input)\n";
	// cin>>filenamee;
	// 	   //cout<<filenamee;
	// send(clientsoc,filenamee,buffer_size,0);
	// int file_size,n;

	// fp1=fopen("3.txt","wb");
	// if(fp1==NULL)
	//      {
	//         perror("Error opening file to write\n");
	       
	//      }
	// memset ( buffer , '\0', BUFF_SIZE);
	// n=recv(clientsoc,buffer,BUFF_SIZE,0); //------write as frst chunk
	// cout<<"Received"<<n<<"bytes\n";
	// cout<<buffer<<"\n";
	// size_t rt=fwrite(buffer,sizeof(char),n,fp1);
	// memset ( buffer , '\0', BUFF_SIZE);
	// fclose(fp1);
	// close(clientsoc);
	// pthread_exit(NULL);

}	

void* service_reqserv(void* argus)
{
	char req_client[1000];
	char req_clientport[200];
	char commandname[20];
	int filsize;
	char filenme[buffer_size];//----------------------use buffer_size evrywhere
	char Buffer [ BUFF_SIZE] ; 
	FILE *fp;
	//pthread_mutex_lock(&mutex);
	
	int n;
	int newsock=*((int *)argus);
	// recv(newsock,(void*)req_client,1000,0);
	// cout<<req_client<<"\n";
	memset(req_client,0,sizeof(req_client));
	// recv(newsock,(void*)req_clientport,200,0);
	// cout<<req_clientport<<endl;
	//cout<< "Client Connected\n";
	//cout<<"waiting for file name\n";
	recv(newsock,commandname,buffer_size,0);
	cout<<commandname<<"\n";
	fflush(stdin);
	memset(commandname,0,sizeof(commandname));
	recv(newsock,filenme,buffer_size,0);
	cout<<filenme<<endl;
	fflush(stdout);
	fp=fopen(filenme,"rb");//----------------send file to client
	if(fp==NULL)
	{
		cout<<"error opening file to be downloaded\n";
	}
	// fseek(fp,0,SEEK_END);
	// filsize=ftell(fp);
	// // rewind(fp);
	// cout<<filsize<<"filwsize\n";
	//fflush(stdout);
	// int x=10;
	// send ( newsock, &x, sizeof(x), 0);
	//-----chunk 1
	//send(newsock,chunkno,sizeof())
	memset ( Buffer , '\0', BUFF_SIZE);
	n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp ) ;
		//cout<<Buffer<<"\n";
		//Buffer[BUFF_SIZE-1]='\0';
	if(send(newsock, Buffer, n, 0 ))
		{
			//cout<<"eror in send()\n";
			perror("Status of send:");
		}
	cout<<n<<" bytes snd\n";
	memset ( Buffer , '\0', BUFF_SIZE);
		//filsize = filsize - n ;
	// 	n=0;
	
	fclose(fp);
	// //pthread_mutex_unlock(&mutex);	
 	//memset(filenme,0,buffer_size);
	close(newsock);

	pthread_exit(NULL);
}



int main(int argc, char* argv[])
{
	if(argc==1)
		cout<<"No extra command line arguments\n";

	pthread_t tid1;
	/*****************CLIENT THREAD**************************************************************/

	if(pthread_create(&tid1, NULL, service_reqclient, NULL)!= 0) 
	{
		cout<<"Failed to create client thread\n";
	} 

	/*******************-----------------------------------SERVER--------------------------------****************/
	pthread_t tid2;
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
	serveraddrss.sin_port = htons( 2000 );
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
    while(newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen))
    {
    
     if(pthread_create(&tid2, NULL, service_reqserv, &newsock)!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    }
	 
	}  

	return 0;
}	
