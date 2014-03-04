#define NVnodes 3
#define NKeys 	65536
#include<vector>
#include<list>
#include<map>
#include<string>

using namespace::std;
typedef unsigned int key_type;
typedef string ip_addr;

class Ktable
{
		public:
				key_type make_new_key();
		private:
				bitmap<key_type> table;

}
class Router
{
		public:
		private:
				map<string,key_type>> keyMap;
				map<key_type,ip_addr> rnodeMap;
}

class Rnode
{
		public:
				Rnode(ip_addr ipaddress):ip(ipaddress);
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

