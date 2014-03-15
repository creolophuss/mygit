#include<iostream>
#include<string>
#include<map>
using namespace::std;

int main(void)
{
		map<int,int> m;

		for(int i = 0,j = 0; j < 10; i+=2 ,j++)
		{
				cout << "i == " << i << endl;
				m[i] = i*i ;
				cout << m[i] << endl;;
		}

		map<int,int>::iterator it;
		for(int i = 1, k = 0; k < 10; i+=2 , k++)
		{
				cout << "i == " << i << endl;
				it = m.lower_bound(i);

				if(it == m.end())
						cout << "end" << endl;
				cout << it->second << endl;
		}
		return 0;
}

