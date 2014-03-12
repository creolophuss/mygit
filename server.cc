#include<iostream>
#include"network.h"


using namespace::std;

int main(void)
{

		Server s("192.168.0.10",6001);
		s.register_router("192.168.0.10",6000);
		s.init_ev();
		s.process_request(s);
		return 0;
}

