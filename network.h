#include<fcntl.h>
#include<arpa/inet.h>
#include<cstring>
#include<string>
#include<sys/epoll.h>
#include<errno.h>
#include<map>
#define MAX_CONNECT_QUEUE	1024
#define MAX_BUF_LEN		512	
#define MAX_LEN			512
#define MAXLINE 512

using namespace::std;

class NetInfo
{
		public:
				NetInfo(string ipaddr,int port_num)
				{
						const char *addr_ptr = ipaddr.c_str();
						short port = (short) port_num;
						addr.sin_family = AF_INET;
						addr.sin_port = htons(port);
						addr.sin_addr.s_addr = inet_addr(addr_ptr);
						memset(&addr.sin_zero, 0, 8);
				}
				int get_fd();
				int set_fd();
				int set_epfd();
				int init_ev();
				string get_mq(int fd);
				void set_mq(int fd,string str);
				int process_request(class NetInfo &server_node);
				virtual void work(int fd){}
				virtual ~NetInfo(){}

		protected:
				map<int,string> msgq;
				struct sockaddr_in addr;
				struct epoll_event ev;
				struct epoll_event events[20];
				int epfd;
				int fd;
};


class Router: public NetInfo
{
		public:
				Router(string ip,int port_num):NetInfo(ip,port_num){}
//				add_new_server();
//				route();
//				rm_server();
//				server_hashing();
				//client_hashing();
				virtual void work(int fd);
				virtual ~Router(){}

		private:
};
class Server: public NetInfo
{
		public:
				Server(string ip,int port_num):NetInfo(ip,port_num){}
				void register_router(string router_ip,int router_port);
//				void init_working_thread_pool();
				virtual void work(int fd);
				virtual ~Server(){}
		private:
};

class Client: public NetInfo
{
		public:
//				route_request(string key);
//				service_request();
		private:
};
