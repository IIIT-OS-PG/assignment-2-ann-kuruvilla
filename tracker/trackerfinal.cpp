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

//--------------------------------./a.out tracker_info.txt trackerno

using namespace std;

#define queuelimit 10
#define BLK_SIZE 10
#define str_size 50

char req_client[1000];
char req_clientport[200];
char usernme[50];
char paswd[50];
char arg3[200];
char arg4[200];



struct peer_info
{
	char ip[9];
	int peer_port;
	char username[str_size];
	char password[str_size];
	int login_flg;//----------------------set this when the user logs in
};

struct group_info{

	 string group_id;
	 int owner_port;
	 int no_of_peers;
};

struct file_info
{
	char filename[50];
	int filesize;
	string sha1;
};

struct peer_info search(vector<struct peer_info> &userlists, string username)
{
	int i;
	for(i=0;i<userlists.size();i++)
	{
		const char* cmp=(const char*)username.c_str();
		if(strcmp(userlists[i].username,cmp)==0)
			return userlists[i];

	}
}
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

vector<struct file_info> filelists;//----------list of files and info regarding file like siz hash
unordered_map< string,int > filesizehash;//--------map of filename and file size
vector<struct peer_info> userlists;//-----------list of all peers online
vector<struct group_info> grouplist;//-----------------list of all the groups in the network
unordered_map<string, vector<string> > peers_in_group;//---------list of all peers in agroup
unordered_map<string, vector<string> > group_files;//----------list of all sharable files in a group
unordered_map< string,vector<string > > peer_lists;//-------------------filename and appended string of (appended)port numbers of peers

void* service_req(void	*arg)
{
	int newsock=*((int *)arg);
	vector<string> rs;
	string portn;
	char portno[5];
	int filesize;
	string st;
	char filehash[2500];
	char peerlist[200];
	char emptybuffr[10];
	strcpy(emptybuffr,"godislove");
	while(1)
	{
			recv(newsock,(void*)req_client,1000,0);
			cout<<req_client<<"\n";
			rs=splitbydelimeter(req_client,';');
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			memset(req_client,0,sizeof(req_client));
		if(rs[0]=="create_user")
		{	
			
			struct peer_info pnew;
			strcpy(usernme,rs[1].c_str());
			strcpy(paswd,rs[2].c_str());
			cout<<"paswd :"<<paswd<<endl;
			strcpy(pnew.username,usernme);
			strcpy(pnew.password,paswd);
			userlists.push_back(pnew);
			rs.clear();
		}
		if(rs[0]=="login")
		{
			struct peer_info nod=search(userlists,rs[1]);
			if(nod.password==rs[2])
			{
				cout<<"Successfully logged in\n";
				nod.login_flg=1;
			}
			else
				cout<<"Wrong password\n";
			rs.clear();
		}
		if(rs[0]=="create_group")
		{
			struct group_info nod;
			nod.group_id=rs[1];
			nod.no_of_peers=1;
			//recv(portno)--------------make this portno owner of the group
			grouplist.push_back(nod);
			cout<<"Group created\n";
			rs.clear();
		}
		if(rs[0]=="upload_file")
		{
			struct file_info nod;
			recv(newsock,portno,5,0);
			//nod.port=portn;
			fflush(stdout);
			 string s="";
			 s=portno;	
			cout<<"Port Number of client who is uploading:";
			fflush(stdout);
			cout<<portno;
			fflush(stdout);
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			recv(newsock,&filesize,sizeof(filesize),0);
			fflush(stdout);
			cout<<"size of file"<<filesize<<endl;
			nod.filesize=filesize;
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			fflush(stdout);
			//recv(newsock,&filesize,sizeof(int),0);
			//fflush(stdout);
			//cout<<endl;
			fflush(stdout);

			if(peer_lists.find(rs[1]) == peer_lists.end())
			{
				vector<string> v1;
				v1.push_back(s);
				peer_lists.insert(make_pair(rs[1],v1));//-------mapping between filenme and port of peers hvng this file
			}
			else 
			 peer_lists[rs[1]].push_back(s);
			// for(int i=0;i<peer_lists.size();i++)
			// 	cout<<peer_lists[i].first<<peer_lists[i].second<<endl;
			strcpy(nod.filename,rs[1].c_str());
			if(group_files.find(rs[2])==group_files.end())
			{
				vector<string> ve1;
			
			 ve1.push_back(rs[1]);
			 group_files.insert(make_pair(rs[2],ve1));
			}
			else
				 group_files[rs[2]].push_back(rs[1]);
			fflush(stdout);

			if(filesizehash.find(rs[1]) == filesizehash.end())
			{

				filesizehash.insert(make_pair(rs[1],filesize));//-----filename and filesize is mapped
			}

			fflush(stdout);
			filesize=0;
			recv(newsock,filehash,2500,0);
			cout<<"Filehash received : "<<filehash<<endl;
			nod.sha1=filehash;
			filelists.push_back(nod);
			memset(filehash,0,sizeof(filehash));
			memset(portno,0,sizeof(portno));
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			//cout//<<nod.sha1<<endl;
			cout<<"File Uploaded\n";
			s="";
			rs.clear();

		}
		if(rs[0]=="list_files")
		{
			cout<<"Files in group id: "<<rs[1]<<endl;

			for(int i=0;i<group_files[rs[1]].size();i++)
				cout<<group_files[rs[1]][i]<<endl;
			rs.clear();
		}

		if(rs[0]=="join_group")
		{
			cout<<"Joining request accepted by Group Owner"<<endl;
			recv(newsock,portno,10,0);
			//cout<<portno<<endl;
			string s1(portno);
			if(peers_in_group.find(rs[1])==peers_in_group.end())
			{
				vector<string> v;
				v.push_back(s1);
				peers_in_group.insert(make_pair(rs[1],v));
			}
			peers_in_group[rs[1]].push_back(s1);//---when a peer joins a group share all its files in this group
			memset(portno,0,sizeof(portno));
			rs.clear();
		}
		if(rs[0]=="download_file")
		{
			send(newsock,emptybuffr,strlen(emptybuffr),0);
			recv(newsock,portno,10,0);//---------------------port no of client sending request fr downloading a file
			string s2=portno;
			cout<<"Client with port number : "<<portno<<" requesting file for download\n";

			//---------see if this portno is a member of group thn only he cn download frm group
			//------yet to implmnt authntication

			if(peer_lists.find(rs[2])!=peer_lists.end())//-------------searching by filename to get all peers having this file
			{
				st="";
				for(auto it=peer_lists[rs[2]].begin(); it!=peer_lists[rs[2]].end();it++)
				{
					st+=*it;

					st+=";";
				}
				
			}
			else
			{
				cout<<"File not present\n";
			}

			strcpy(peerlist,st.c_str());
			send(newsock,peerlist,strlen(peerlist),0);//-------------sending list of peers hvng the file
			//memset(filesize,0,sizeof(filesize));
			filesize=0;
			if(filesizehash.find(rs[2]) != filesizehash.end())
			 filesize=filesizehash[rs[2]];
			recv(newsock,emptybuffr,sizeof(emptybuffr),0);
			send(newsock,&filesize,sizeof(filesize),0);
			filesize=0;
			memset(portno,0,sizeof(portno));
			memset(peerlist,0,sizeof(peerlist));
			rs.clear();
		}



	}
	// else if()
	// {

	// }
	
	// recv(newsock,(void*)arg4,strlen(arg4),0);
	// cout<<arg4<<"helo";
	// memset(arg4,0,sizeof(arg4));
	fflush(stdout);
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