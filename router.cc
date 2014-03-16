#include<iostream>
#include"network.h"

int main(void)
{
		string ipaddr("192.168.1.102");
		Router r(ipaddr,"6000");
		r.init_ev();

		r.process_request(r);
		return 0;
}
