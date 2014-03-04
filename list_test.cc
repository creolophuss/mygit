#include<list>
#include<iostream>
#include<iterator>
using namespace::std;

int main(void)
{
		list<int> l;
		list<int>::iterator iter = l.begin();
		int value;
		
		while( cin >> value)
		{
				list<int>::iterator iter = l.begin();
				if(l.empty())
				{
						l.push_back(value);
						cout << "Empty"<< endl;
				}
				else{
						while(iter != l.end())
						{
								if(value <= *iter)
								{
										l.insert(iter,value);
										break;
								}
								else
										iter++;
						}
						if(iter == l.end())
								l.push_back(value);
				}
		}
		iter = l.begin();
		while(iter != l.end())
		{
				cout << *iter << endl;
				iter++;
		}


		return 0;
}
								


