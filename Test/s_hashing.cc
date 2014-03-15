#include<string>
#include<cstring>
#include<cstdlib>
#include<iostream>
#define SPACE_SIZE 65536
using namespace::std;
int s_hash(string ip,string port);
int c_hash(string ip);
void rm_ch(char *str, char c);
int main(void)
{
		string ip;
		string port;
		int i = -1;
		cout <<"Size of int == " << sizeof(int) << endl;
		cout << (unsigned)i << endl;
/*		while(1)
		{
				cout << "Input IP : ";
				cin >> ip;
				cout << "Input PORT: ";
				cin >> port;
				int key =s_hash(ip,port);
				cout << "Key : " << key << endl;
		}
		*/
		while(1)
		{
				cout << "Input  : ";
				cin >> ip;
				int key = c_hash(ip);
				cout << key << endl;
		}
		return 0;
}

int s_hash(string ip,string port)
{
		char buffer[64];
		strcpy(buffer,ip.c_str());
		rm_ch(buffer,'.');
		cout << "Buffer : " << buffer << endl;
		int a = atoi(buffer);
		int b = atoi(port.c_str());
		int key = a * b % SPACE_SIZE;
		return key;
}
		
void rm_ch(char *str, char c)
{
		char *p,*q;
		p = q = str;
		while(*q != '\0')
		{
				if( *p != c  )
						p++;
				q++;
				while( *q == c )
						q++;
				*p = *q;
		}
}	

int c_hash(string str)
{
		char buffer[64];
		const char *p = str.c_str();
		int key= 1;
		while(*p != '\0')
		{
				cout << *p ;
				key *= (int)(*p);
				key %= SPACE_SIZE;
				p++;
		}
		cout << endl;
		return key;
}


