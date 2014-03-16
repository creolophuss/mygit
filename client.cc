#include<iostream>
#include"network.h"
using namespace::std;

int main(void)
{
		Client c;
		string rqst;
		c.login("192.168.1.102","6000");

		while(1)
		{
				cout << "HDB:> ";
				string s_info;
				cin >> rqst;
				if(rqst == "exit")
						break;
				s_info = c.route_request(rqst);
				cout << c.service_request(s_info,rqst);
		}
		return 0;
}
