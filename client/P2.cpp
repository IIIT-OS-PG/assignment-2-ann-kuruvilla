#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
//#include <string>
#include <pthread.h>

#define BUFF_SIZE 50
#define queuelimit 30
#define CHUNK_SIZE 512

using namespace std;

vector<string> splitbydelimeter(string s,char deli)
{
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
void* service_reqserv(void* argus)
{
	char str[50]= "127.0.0.1:3058=C1,C2,C3,C4";
	int newsock=*((int *)argus);
	send(newsock,str,strlen(str),0);


}


void* write_chunks(void* arggs)
{
	int i;
	// std::string *sp = static_cast<std::string*>(arggs);
	// string port= *sp;
	int port=*((int *)arggs);
	cout<<port<<"port no\n";
	//const char portno[port.size()+1];
	//strcpy(portno,port.c_str());
	//cout<<portno;

	// string portno=port;
	// char portnoo[portno.size()];
	// for(i=0;i<portno.size();i++)
	// 	portnoo[i]=portno[i];

	// int portno=stoi(port);
	// cout<<"Connecting to peer with IP 127.0.0.1 and port "<<portno<<endl;
	int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	if(clientsoc  < 0)
	{
		perror("Client socket creation failed\n");
		exit(1);
	}
	char chunkstring[BUFF_SIZE];
	int filesize;
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	//unsigned short portnum = (unsigned short) strtoul(portnoo, NULL, 0);
	serv_addr.sin_port = htons(port);
	int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect to server failed for client\n");
    	exit(1);
    }

    cout<<"Client received ";
    recv(clientsoc,chunkstring,BUFF_SIZE,0);
    recv(clientsoc,&filesize,sizeof(filesize),0);
    cout<<"IP port and chunk info from tracker\n";
    //cout<<chunkstring<<endl;
    cout<<filesize<<endl;
    fflush(stdout);
    FILE *fpt=fopen("4.txt","wb");
  	int X=filesize-1;
  	fseek(fpt, X , SEEK_SET);
    fputc('\0', fpt);
    fclose(fpt);
    //----------------------------------------extracting chunk details----------
    
    char deli;
    string s=chunkstring;
    vector<string> parts;
    deli='=';
	string st="";
	for(i=0;i<s.size();i++)
	{
		if(s[i]!=deli)
		{
			st+=s[i];
		}
		else
		{
			parts.push_back(st);
			st="";
		}

	}
	parts.push_back(st);
	st="";
    string ip_port_pair=parts[0];
    //cout<<ip_port_pair<<endl;
    s=parts[1];//------------------------lists of chunks string
    deli=',';
    vector<string> parts2;
    for(i=0;i<s.size();i++)
    {
    	if(s[i]!=deli)
		{
			st+=s[i];
		}
		else
		{
			parts2.push_back(st);
			st="";
		}
    }
    parts2.push_back(st);
    
    cout<<"Chunks sending...\n";
    //cout<<parts2[0]<<parts2[1]<<parts2[2];
    fflush(stdin);
    char chunkid[2];
    char bufferc[CHUNK_SIZE];
    int chunkposc;
    fpt=fopen("4.txt","wb");
    //-------------------------------piece selection -----asking for chunks sequentially
  	for(i=0;i<parts2.size();i++)
  	{	
  		strcpy(chunkid,parts2[i].c_str());
  		cout<<"Client asking for "<<chunkid<<endl;
  		send(clientsoc,chunkid,2,0);
  		chunkposc= chunkid[1] - '0';
  		memset(chunkid,0,sizeof(chunkid));
  		recv(clientsoc,bufferc,CHUNK_SIZE,0);
  		//cout<<bufferc<<endl;
  		fseek(fpt,(chunkposc-1)*CHUNK_SIZE,SEEK_SET);
  		fwrite(bufferc,sizeof(char),CHUNK_SIZE,fpt);
  		memset(bufferc,0,CHUNK_SIZE);
  	}
  	cout<<"Chunks written\n";
  	

  	fclose(fpt);
	close(clientsoc);
}


/*---------------------------------P2 Peer System-------------------------------*/
void* service_reqclient(void* args)
{
	cout<<"client\n";
	char command[100];
	cin>>command;
	//char deli;
	int i;
	//char peer_info[200]="127.0.0.1:3058,127.0.0.1:3625";
	char peer_info[200]="3058,3625";
	int peer[10];
	//char chunk_info[]="3058=C1,C3,C5";
	pthread_t tidc[10];
	int number_of_peers;
	string peerinfo=peer_info;
	string strr;
	vector<string> peers;
	char deli=',';

	 for(i=0;i<peerinfo.size();i++)//-----------------split peer_info find numbr of peers----complte this fn
	 {
	 	if(peerinfo[i]!=deli)
	 	{
	 		strr+= peerinfo[i];
	 	}
	 	else
	 	{
	 		peers.push_back(strr);
	 		strr="";
	 	}
	 }
	 peers.push_back(strr);
	 
	number_of_peers=peers.size();
	
	for(i=0;i<number_of_peers;i++)
	{
		peer[i]=stoi(peers[i]);
		//cout<<peer<<"peerport";
		pthread_create(&tidc[i],NULL,write_chunks,(void*)&peer[i]);

	}


	// int clientsoc = socket(PF_INET,SOCK_STREAM, 0);
	// if(clientsoc  < 0)
	// {
	// 	perror("Client socket creation failed\n");
	// 	exit(1);
	// }
	// char chunkstring[BUFF_SIZE];
	// int filesize;
	// struct sockaddr_in serv_addr;
	// memset(&serv_addr, '\0', sizeof(serv_addr));
	// serv_addr.sin_family = AF_INET; 
	// serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	// serv_addr.sin_port = htons(3058);
	// int re= connect(clientsoc,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
 //    if(re<0)
 //    {
 //    	perror("connect to server failed for client\n");
 //    	exit(1);
 //    }

 //    cout<<"Client received ";
 //    recv(clientsoc,chunkstring,BUFF_SIZE,0);
 //    recv(clientsoc,&filesize,sizeof(filesize),0);
 //    cout<<"IP port and chunk info from tracker\n";
 //    //cout<<chunkstring<<endl;
 //    cout<<filesize<<endl;
 //    fflush(stdout);
 //    FILE *fpt=fopen("4.txt","wb");
 //  	int X=filesize-1;
 //  	fseek(fpt, X , SEEK_SET);
 //    fputc('\0', fpt);
 //    fclose(fpt);
 //    //----------------------------------------extracting chunk details----------
 //    string s=chunkstring;
 //    vector<string> parts;
 //    deli='=';
	// string st="";
	// for(i=0;i<s.size();i++)
	// {
	// 	if(s[i]!=deli)
	// 	{
	// 		st+=s[i];
	// 	}
	// 	else
	// 	{
	// 		parts.push_back(st);
	// 		st="";
	// 	}

	// }
	// parts.push_back(st);
	// st="";
 //    string ip_port_pair=parts[0];
 //    //cout<<ip_port_pair<<endl;
 //    s=parts[1];//------------------------lists of chunks string
 //    deli=',';
 //    vector<string> parts2;
 //    for(i=0;i<s.size();i++)
 //    {
 //    	if(s[i]!=deli)
	// 	{
	// 		st+=s[i];
	// 	}
	// 	else
	// 	{
	// 		parts2.push_back(st);
	// 		st="";
	// 	}
 //    }
 //    parts2.push_back(st);
    
 //    cout<<"Chunks sending...\n";
 //    //cout<<parts2[0]<<parts2[1]<<parts2[2];
 //    fflush(stdin);
 //    char chunkid[2];
 //    char bufferc[CHUNK_SIZE];
 //    int chunkposc;
 //    fpt=fopen("4.txt","wb");
 //    //-------------------------------piece selection -----asking for chunks sequentially
 //  	for(i=0;i<parts2.size();i++)
 //  	{	
 //  		strcpy(chunkid,parts2[i].c_str());
 //  		cout<<"Client asking for "<<chunkid<<endl;
 //  		send(clientsoc,chunkid,2,0);
 //  		chunkposc= chunkid[1] - '0';
 //  		memset(chunkid,0,sizeof(chunkid));
 //  		recv(clientsoc,bufferc,CHUNK_SIZE,0);
 //  		//cout<<bufferc<<endl;
 //  		fseek(fpt,(chunkposc-1)*CHUNK_SIZE,SEEK_SET);
 //  		fwrite(bufferc,sizeof(char),CHUNK_SIZE,fpt);
 //  		memset(bufferc,0,CHUNK_SIZE);
 //  	}
 //  	cout<<"Chunks written\n";
  	

 //  	fclose(fpt);
	// close(clientsoc);
	pthread_exit(NULL);
	
 }

int main(int argc, char* argv[])
{

	/*****************CLIENT THREAD**************************************************************/
	pthread_t tid1;
	if(pthread_create(&tid1, NULL, service_reqclient, NULL)!= 0) 
	{
		cout<<"Failed to create client thread\n";
	} 
	
	/*******************-----------------------------------SERVER--------------------------------****************/

	pthread_t tid2;
	struct sockaddr_in serveraddrss;
	struct sockaddr_in newaddrs;
	socklen_t addrlen;
	int newsock,i;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	
	// vector<string> parts;

	// parts=splitbydelimeter(str,'=');
	// string ip_port_pair=parts[0];
	// parts2 = splitbydelimeter(parts[1],',');//----------------parts2 contain C1,C2,C3,C4(chunk ids)




	if(socketfd < 0)	//tcp -sock_stream  af_inet - ipv4
	{
		perror("socket failed\n");
		exit(1);
	}
	cout<<"Socket created\n";
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
	serveraddrss.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddrss.sin_port = htons( 2025 );
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

    if(newsock = accept(socketfd,(struct sockaddr*) &newaddrs, (socklen_t*) &addrlen))
    {
    //send(newsock,str,strlen(str),0);
	    if(pthread_create(&tid2,NULL,service_reqserv,&newsock)!=0)
	    {
	    	cout<<"Failed to create server request service thread\n";
	    }
	}


    //pthread_join()
	pthread_exit(NULL);
	return 0;

}		