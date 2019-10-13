// C++ program to words in a sentence. 
#include <bits/stdc++.h> 
using namespace std; 

void removeDupWord(char str[100]) 
{ 
	// Returns first token 
	char *token = strtok(str, " "); 

	// Keep printing tokens while one of the 
	// delimiters present in str[]. 
	cout<<token;
} 

// Driver function 
int main() 
{ 
	char str[100] ;
	cin>>str;
	removeDupWord(str); 
	return 0; 
}
