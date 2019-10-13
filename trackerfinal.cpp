#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

//-------------------------------------g++ trackerfinal.cpp -lpthread ------to compile
//-------------------------------------./a.out tracker_info.txt 1


using namespace std;

#define queuelimit 5
#define BLK_SIZE 10

char req_client[1000];
char req_clientport[200];
char commandname[200];
void* service_req(void	*arg)
{
	int newsock=*((int *)arg);
	recv(newsock,(void*)req_client,1000,0);
	cout<<req_client<<"\n";
	memset(req_client,0,sizeof(req_client));
	recv(newsock,(void*)req_clientport,200,0);
	cout<<req_clientport<< "Client Connected";
	cout<<"helo";
	fflush(stdout);
	// ssize_t nu=recv(newsock,(void*)commandname,200,0);
	// if(nu==-1)
	// 	perror("not received commandname from client\n");

	// cout<<"hello"<<commandname<<endl;
	//memset(commandname,0,sizeof(commandname));
	//pthread_exit(NULL);

}

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
    pthread_t tid[50];
    int i=0;
  //  while(1)
  //  { 	
	    addrlen= sizeof(newaddrs);
	    newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen); //accept is a blckng call
	    if(pthread_create(&tid[i], NULL, service_req, &newsock)!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    }

	    
	 pthread_join(tid[i],NULL);
	   // pthread_detach(tid[i]);
	   cout<<"thread fn done\n"; 
	    
	//}    

   //cout<<

  
   
    //close(newsock);


    close(socketfd);
	return 0;
}