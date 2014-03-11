#include<fcntl.h>
#include<arpa/inet.h>
#include<cstring>
#include<string>

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
				int get_fd()
				{
						return fd;
				}
				int set_fd()
				{
						fd = socket(PF_INET,SOCK_STREAM,0);
						return fd;
				}
				int set_epfd()
				{
						epfd = epoll_create(256);
						return epfd;
				}
				int init_ev()
				{
						set_fd();
						int ret = bind(fd,(struct sockaddr *)&addr,sizeof(struct sockaddr));
						if(ret == -1)
						{
								perror("Bind error");
								close(fd);
								exit(-1);
						}
						else if(ret == 0)
								cout << "Bind OK" << endl;
						make_socket_non_blocking(fd);

						epfd = epoll_create(256);
						ev.data.fd = fd;
						ev.events = EPOLLIN | EPOLLET;
						epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
						listen(fd,MAX_CONNECT_QUEUE);

						return 0;
				}
				string get_msg(int fd)
				{
						return mq[fd];
				}
				void set_mq(int fd,string str)
				{
						mq[fd] = str;
				}

				int process_request(class NetInfo &server_node);



		protected:
				struct sockaddr_in addr;
				struct epoll_event ev;
				struct epoll_event events[20];
				int epfd;
				int fd;
				map<int fd,string msg> mq;
}
class Router: public NetInfo
{
		public:
				Router(string ip,int port_num):net_info(ip,port_num){}
				init_network();
				add_new_server();
				route();
				rm_server();
				server_hashing();
				client_hashing();

		private:
}
class Server: public NetInfo
{
		public:
				Server(string ip,int port_num):net_info(ip,port_num){}
				void register_router(string router_ip,int router_port);
				init_working_thread_pool();
		private:
}
class Client: public NetInfo
{
		public:
				route_request(string key);
				service_request();
		private:
}
