#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <pthread.h>
using namespace std;

#define queuelimit 10
#define BLK_SIZE 10
#define str_size 50
vector<string> splitbydelimeter(string s,char deli)
{
	//cout<<"Inside splitbydelim"<<endl;
	int i;
	vector<string> v;
	string st="";
	for(i=0;i<s.size();i++)
	{
		if(s[i]!=deli)
		{
			st+=s[i];
		}
		else
		{
			v.push_back(st);
			st="";
		}

	}
	v.push_back(st);
	st="";

return v;	
}

void* service_req(void	*arg)
{
	int newsock=*((int *)arg);
	char req_client[1000];
	vector<string> rs;
	char portno[5];
	int filesize;
	char filehash[2500];
	char emptybuffr[10];
	strcpy(emptybuffr,"godislove");
	while(1)
	{
		recv(newsock,(void*)req_client,1000,0);
			cout<<req_client<<"\n";
		rs=splitbydelimeter(req_client,';');
		memset(req_client,0,sizeof(req_client));
		if(rs[0]=="upload_file")
		{
			recv(newsock,portno,5,0);
			cout<<"Port Number of client who is uploading:";
			cout<<portno;
			fflush(stdout);
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			recv(newsock,&filesize,sizeof(filesize),0);
			cout<<"size of file"<<filesize<<endl;
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			recv(newsock,filehash,2500,0);
			cout<<"Filehash received : "<<filehash<<endl;
			memset(filehash,0,sizeof(filehash));
			memset(portno,0,sizeof(portno));
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			cout<<"File Uploaded\n";
			rs.clear();

		}	

	}
	pthread_exit(NULL);
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
    cout<<"Port number of tracker: "<<number<<endl;
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
    pthread_t tid[15];
    int i=0;
     addrlen= sizeof(newaddrs);
     while(newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen))
     { 	
	   
	     //accept is a blckng call
	    if(pthread_create(&tid[i++], NULL, service_req, &newsock)!= 0 )
	    {
	    	cout<<"failed to create thread\n";
	    } 
	    
	}    
 
 
   
    close(socketfd);
    pthread_exit(NULL);
	return 0;
}