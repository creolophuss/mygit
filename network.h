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
						fd = socket(PF_INET,SOCK_STREAM,0);
				}


		private:
				struct sockaddr_in addr;
				int fd;
}
class Router
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
				NetInfo net_info;


}
class Server
{
		public:
				Server(string ip,int port_num):net_info(ip,port_num){}
				init_network();
				register_router();
				init_working_thread_pool();
		private:
				NetInfo net_info;
}
class Client
{
		public:
				route_request();
				service_request();
		private:
				NetInfo net_info;
}
