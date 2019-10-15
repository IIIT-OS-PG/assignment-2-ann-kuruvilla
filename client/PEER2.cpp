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
#include <openssl/sha.h>
#include <pthread.h>

#define BUFF_SIZE 50
#define queuelimit 30
#define CHUNK_SIZE 512


using namespace std;

char port[5];


int calc_filesiz(string filname)
{
	int filesize;
	char filo[50];
	strcpy(filo,filname.c_str());
	FILE * fp=fopen(filo,"rb");
	fseek(fp,0,SEEK_END);
	filesize=ftell(fp);
	 fclose(fp);
return filesize;	 
}

vector<string> splitbydelimeter(string s,char deli)
{
	int i;
	vector<string> v;
	string st="";
	for(i=0;i<s.size()-1;i++)
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
	char str[50]= "127.0.0.1:2306=C1,C2,C3,C6,C7";
	int newsock=*((int *)argus);
	send(newsock,str,strlen(str),0);
	int filsize;
	char chunkidd[2];
	FILE *fp=fopen("1.txt","rb");
	char buffer[CHUNK_SIZE];
	int n,chunkpos;
	while(recv(newsock,chunkidd,2,0))
	{
		cout<<chunkidd<<endl;
		fflush(stdout);
		chunkpos= chunkidd[1] - '0';
		if(chunkpos == 0)//--------------------------already received chunk
			continue;
		memset(chunkidd,0,2);
		fseek(fp,(chunkpos-1)*CHUNK_SIZE,SEEK_SET);
		n=fread(buffer,sizeof(char),CHUNK_SIZE,fp);
		send(newsock,buffer,n,0);
		memset(buffer,0,CHUNK_SIZE);
		n=0;
	}
close(newsock);


}


void* write_chunks(void* arggs)
{
	int i;
	vector<int> chunksreceived(10);
	int port=*((int *)arggs);
	cout<<port<<"port no\n";
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

    //cout<<"Client received ";
    recv(clientsoc,chunkstring,BUFF_SIZE,0);//the chunk info is received
    //recv(clientsoc,&filesize,sizeof(filesize),0);
    cout<<"Chunk info received from peer\n";
    //cout<<chunkstring<<endl;
    //cout<<filesize<<endl;
    fflush(stdout);
   //  FILE *fpt=fopen("4.txt","wb");
  	// int X=filesize-1;
  	// fseek(fpt, X , SEEK_SET);
   //  fputc('\0', fpt);
   //  fclose(fpt);
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
    FILE* fpt=fopen("4.txt","wb");
    //-------------------------------piece selection -----asking for chunks 
  	for(i=0;i<parts2.size();i++)
  	{	
  		strcpy(chunkid,parts2[i].c_str());
  		cout<<"Client asking for "<<chunkid<<endl;
  		chunkposc= chunkid[1] - '0';
  		if(chunksreceived[chunkposc]==0)
  		 send(clientsoc,chunkid,2,0);//--------------------asking for this chunk from peer
  		else
  		{
  			string t="C0";
  			strcpy(chunkid,t.c_str());
  			send(clientsoc,chunkid,2,0);
  			memset(chunkid,0,sizeof(chunkid));
  			continue;
  		}
  		memset(chunkid,0,sizeof(chunkid));
  		recv(clientsoc,bufferc,CHUNK_SIZE,0);
  		//cout<<bufferc<<endl;
  		fseek(fpt,(chunkposc-1)*CHUNK_SIZE,SEEK_SET);
  		fwrite(bufferc,sizeof(char),CHUNK_SIZE,fpt);
  		chunksreceived[chunkposc]=1;
  		memset(bufferc,0,CHUNK_SIZE);
  	}
  	cout<<"Chunks written\n";
  	

  	fclose(fpt);
	close(clientsoc);
}


string filehash(char filenm[100])
{
	FILE *f;
	int i;
	f=fopen(filenm,"rb");

	if(f==NULL)
	{
		perror("Error opening file for sha calculation\n");
		exit(1);
	}
	char buf[CHUNK_SIZE];
	char outputBuffer[70];
	size_t sz;
	SHA_CTX md;
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Init(&md);

	int blk=0;

	string str="";
	while(!feof(f))
	{
		if((sz=fread((void*)buf,1,CHUNK_SIZE,f ))>0) //reads a piece of size maxblks in 1 read and finds its hash
		{
			SHA1_Init(&md);
			SHA1_Update(&md,buf,sz);	
			SHA1_Final(hash,&md);
			blk++;
			for(i = 0; i < SHA_DIGEST_LENGTH; i++)
	    	{
	        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);// hash in outputBuffer
	    	}
	    	//cout<<outputBuffer<<"\n";
	    	for(i=0;i<20;i++)
	    	str+=outputBuffer[i];
	    	memset(outputBuffer,0,sizeof(outputBuffer));
			memset(hash,0,sizeof(hash));
			

		}
	}
	fflush(stdin);
	cout<<endl<<str<<"hash of totl file"<<endl;
	fflush(stdout);
	fclose(f);
return str;	

}	


/*---------------------------------P1 Peer System-------------------------------*/
void* service_reqclient(void* args)
{
	FILE* fp;
	FILE* fpt;
	cout<<"client\n";
	int clientsock = socket(PF_INET,SOCK_STREAM, 0);
	if(clientsock  < 0)
	{
		perror("Client socket creation failed\n");
		exit(1);
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	serv_addr.sin_port = htons(2000);//----------------------------------connecting to tracker; tracker ip and port
	int re= connect(clientsock,(struct sockaddr*) &serv_addr,sizeof(serv_addr));
    if(re<0)
    {
    	perror("connect to server failed for client\n");
    	exit(1);
    }
    cout<<"Connected to tracker\n";
	int option;
	char command[100];
	char fil[50];
	string filename;
	vector<string> rs;
	vector<string> pe;
	int filesiz;
	int filesize;
	int number_of_peers;
	string hashs;
	char hashc[2048];
	char peerlists[200];
	string peerlist;
	int peer[10];
	int i,X;
	char emptybuff[10];
	char emptybuff2[10];
	strcpy(emptybuff2,"godislove");
	pthread_t tidc[10];
	//char password[100];
	cout<<"Enter option and command separated by ; \n";
	cin>>option;
	cin>>command;//----------------------create_user username passwd----------
	 while(1)
	 {
		 switch(option)
		{
			case 1:send(clientsock,command,100,0);
				   cout<<"Create User Account\n";
				   recv(clientsock,emptybuff,10,0);
				   memset(command,0,sizeof(command));
				   memset(emptybuff,0,sizeof(emptybuff));
				   cout<<"User Port Number: "<<port<<endl;
				   send(clientsock,port,strlen(port),0);
				   recv(clientsock,emptybuff,10,0);
				   break;
			case 2:send(clientsock,command,100,0);
					cout<<"User Login\n";
					memset(command,0,sizeof(command));
					break;
			case 3:send(clientsock,command,100,0);
					cout<<"Create Group\n";
					recv(clientsock,emptybuff,10,0);
					memset(emptybuff,0,sizeof(emptybuff));
					send(clientsock,port,strlen(port),0);
					recv(clientsock,emptybuff,10,0);
					memset(command,0,sizeof(command));
					break;
			case 4:send(clientsock,command,100,0);//------------------join_group;G1
					recv(clientsock,emptybuff,100,0);
					send(clientsock,port,strlen(port),0);
					recv(clientsock,emptybuff,10,0);
					memset(command,0,sizeof(command));
					break;
			case 5:send(clientsock,command,100,0);//---------------leave_group
					recv(clientsock,emptybuff,100,0);
					send(clientsock,port,strlen(port),0);
					recv(clientsock,emptybuff,10,0);
					memset(command,0,sizeof(command));
					break;

			case 6:send(clientsock,command,100,0);//-------------upload command----upload_file;1.txt;G1;6789
					cout<<"Uploading file\n";
					//cout<<port<<endl;
					recv(clientsock,emptybuff,100,0);
					send(clientsock,port,strlen(port),0);//----------sending peer port number
					fflush(stdout);
					rs=splitbydelimeter(command,';');
					filename=rs[1];
					memset(fil,0,sizeof(fil));
					recv(clientsock,emptybuff,100,0);
					strcpy(fil,filename.c_str());
					filename="";
					filesiz=0;
					fp=fopen(fil,"rb");
					if(fp==NULL)
					{
						cout<<"File not opened successfully for calculating size\n";
					}
					fseek(fp,0,SEEK_END);
					filesiz=ftell(fp);
	 				fclose(fp);
					send(clientsock,&filesiz,sizeof(filesiz),0);
					cout<<filesiz<<"filesize\n";
					recv(clientsock,emptybuff,100,0);
					filesiz=0;
					//send(clientsock,&filesiz,sizeof(int),0);
					fflush(stdout);
					hashs=filehash(fil);
					fflush(stdout);
					memset(hashc,0,sizeof(hashc));
					strcpy(hashc,hashs.c_str());
					hashs="";
					send(clientsock,hashc,strlen(hashc),0);
					fflush(stdout);
					memset(command,0,sizeof(command));
					memset(hashc,0,sizeof(hashc));
					filesiz=0;
					recv(clientsock,emptybuff,100,0);
					fflush(stdout);
					rs.clear();
					break;
			case 7: send(clientsock,command,100,0);
					//cout<<"list_files";//------------------------list_file;G1
					memset(command,0,sizeof(command));
					rs.clear();
					break;
			case 8:send(clientsock,command,100,0);//-------------------join_group
					send(clientsock,port,strlen(port),0);
					
					memset(command,0,sizeof(command));
					rs.clear();
					break;
			case 9:send(clientsock,command,200,0);	//------------download_file			
				   memset(command,0,sizeof(command));
				   recv(clientsock,emptybuff,100,0);
				   send(clientsock,port,strlen(port),0);
				   recv(clientsock,peerlists,200,0);
				   string peerlist=peerlists;
				   pe=splitbydelimeter(peerlist,';');
				   memset(peerlists,0,sizeof(peerlists));
				   number_of_peers=pe.size();
				   cout<<number_of_peers<<" ->No of peers hvng the file requested\n";
				   filesize=0;
				   send(clientsock,emptybuff2,sizeof(emptybuff2),0);
				   recv(clientsock,&filesize,sizeof(filesize),0);//-----------------------------filesize from tracker
				   cout<<filesize<<"-> Filesize received from tracker\n";
				    fpt=fopen("4.txt","wb");
				     if(fpt==NULL)
				     {
				     	cout<<"Error opening file\n";
				     }
				  	 X=filesize-1;
				  	fseek(fpt, X , SEEK_SET);
				    fputc('\0', fpt);
				    fclose(fpt);
				   for(i=0;i<number_of_peers;i++)
					{
						peer[i]=stoi(pe[i]);
						//cout<<peer<<"peerport";
						pthread_create(&tidc[i],NULL,write_chunks,(void*)&peer[i]);

					}
				   pe.clear();
				   rs.clear();
				   break;
		
			} 

		//}
		cout<<"Enter option and command\n";
		fflush(stdout);
		fflush(stdin);
		cin>>option;
		cin>>command;
		fflush(stdout); 
	}
	

	//char peer_info[200]="127.0.0.1:3058,127.0.0.1:3625";
	//char peer_info[200]="3058,3625";
	
	//char chunk_info[]="3058=C1,C3,C5";
	
	
	
	// string strr;
	// vector<string> peers;
	// char deli=',';

	//  for(i=0;i<peerinfo.size();i++)//-----------------split peer_info find numbr of peers
	//  {
	//  	if(peerinfo[i]!=deli)
	//  	{
	//  		strr+= peerinfo[i];
	//  	}
	//  	else
	//  	{
	//  		peers.push_back(strr);
	//  		strr="";
	//  	}
	//  }
	//  peers.push_back(strr);
	 
	// number_of_peers=peers.size();
	
	// for(i=0;i<number_of_peers;i++)
	// {
	// 	peer[i]=stoi(peers[i]);
	// 	//cout<<peer<<"peerport";
	// 	pthread_create(&tidc[i],NULL,write_chunks,(void*)&peer[i]);

	// }


	 pthread_exit(NULL);
	
 }

int main(int argc, char* argv[])
{

	 //portno=atoi(argv[2]);

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
	
	if(socketfd < 0)	//tcp -sock_stream  af_inet - ipv4
	{
		perror("socket failed\n");
		exit(1);
	}
	cout<<"Socket created\n";
	memset(&serveraddrss, '\0', sizeof(serveraddrss));
	serveraddrss.sin_family = AF_INET; 
	serveraddrss.sin_addr.s_addr = inet_addr(argv[1]);
	unsigned short number = (unsigned short) strtoul(argv[2], NULL, 0);
	strcpy(port,argv[2]);
	//cout<<port;
	serveraddrss.sin_port = htons( number );
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