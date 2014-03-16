#include<iostream>
#include"network.h"


using namespace::std;

int main(void)
{

		Server s("192.168.1.102","7000");
		s.register_router("192.168.1.102",6000);
		s.init_ev();
		s.process_request(s);
		return 0;
}

