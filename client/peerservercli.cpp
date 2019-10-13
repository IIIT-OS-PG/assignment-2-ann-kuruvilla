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
	string in;
	cout<<"client enter command\n";
	cin>>in;


}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* service_reqchildserv(void* argus)
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
	recv(newsock,(void*)req_client,1000,0);
	cout<<req_client<<"\n";
	memset(req_client,0,sizeof(req_client));
	recv(newsock,(void*)req_clientport,200,0);
	cout<<req_clientport<<endl;
	//cout<< "Client Connected\n";
	//cout<<"waiting for file name\n";
	recv(newsock,commandname,buffer_size,0);
	cout<<commandname<<"\n";
	memset(commandname,0,sizeof(commandname));
	recv(newsock,filenme,buffer_size,0);
	cout<<filenme<<endl;
	
	fp=fopen("1.txt","rb");//----------------send file
	if(fp==NULL)
	{
		cout<<"error opening file to be downloaded\n";
	}
	// fseek(fp,0,SEEK_END);
	//  filsize=ftell(fp);
	// rewind(fp);
	///send ( newsock, &filsize, sizeof(filsize), 0);
	//-----chunk 1
	memset ( Buffer , '\0', BUFF_SIZE);
	 n = fread( Buffer , sizeof(char) , BUFF_SIZE-1 , fp ) ;//&& filsize > 0 )
		//cout<<Buffer<<"\n";
		//Buffer[BUFF_SIZE-1]='\0';
		if(send(newsock, Buffer, n, 0 ))
		{
			//cout<<"eror in send()\n";
			perror("err");
		}
		cout<<n<<" bytes snd\n";
		memset ( Buffer , 0, BUFF_SIZE);
		//filsize = filsize - n ;
		n=0;
	
	fclose(fp);
//pthread_mutex_unlock(&mutex);	
 memset(filenme,0,buffer_size);


pthread_exit(NULL);

}
void* service_reqserver(void	*arg)
{
	pthread_t tid3;
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
	cout<<"socket created\n";
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
    
    //newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen);
    if(pthread_create(&tid3, NULL, service_reqchildserv, &newsock)!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    }
	//pthread_join(tid3,NULL); 
	}
	         
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
	if(pthread_create(&tid2, NULL, service_reqclient, NULL)!= 0) 
	{
		cout<<"failed to create thread\n";
	}   

	    

	pthread_join(tid1,NULL); 
	pthread_join(tid2,NULL); 

	return 0;

}