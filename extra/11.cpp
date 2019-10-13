#include <iostream>
#include <unistd.h>

using namespace std;
int main()
{
	int x,y;
	int f=fork();
	if(f==0)
	{
	  cout<<"servng";
	  exit(1);
	}

	cout<<"clint";
    cin>>y;

	

return 0;
}