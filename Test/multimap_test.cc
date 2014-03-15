#include<map>
#include<iostream>
using namespace::std;

int main(void)
{
		multimap<int ,string> m;
		m.insert({1,string("Hello,world")});
		multimap<int ,string>::iterator it;
		it = m.find(1);
		cout << it->second << endl;
		return 0;
}
