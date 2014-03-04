#include"consistent_hashing.h"
typedef unsigned int key_type;
typedef string ip_addr;

class Router
{
		public:
				add_to_keyMap();
				add_to_rnodeMap();
		private:
				map<string,key_type>> keyMap;
				map<key_type,ip_addr> rnodeMap;
}
Router::Router();
Router::add_to_KeyMap();
Router::add_to_rnodeMap();

class Rnode
{
		public:
				Rnode(ip_addr ipaddress):ip(ipaddress){

				}
				register_router(Router &rt);
		private:
				ip_addr ip;
				vector<key_type>	vnodeKeys;
}
int main(void)
{

		initiate Caches;
		add rnode;
		make vnodes;
		link_vnode->rnode;
		comunicate;
}
