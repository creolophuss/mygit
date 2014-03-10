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
				void register_router(string router_ip,int router_port);
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
int Server::register_router(string router_ip,int router_port)
{
		struct sockaddr_in routeraddr;
		routeraddr.sin_family = AF_INET;
		routeraddr.sin_port = htons((short)router_port);
		routeraddr.sin_addr.s_addr = inet_addr(router_ip.c_str());
		memset(&routeraddr.sin_zero, 0, 8);

		int rgst_fd = net_info.set_fd();
		int ret = connect(rgst_fd,struct sockaddr *)&routeraddr,sizeof(struct sockaddr));
		if(ret != 0)
		{
				cout << "connect error while registering. " << endl;
				exit(-1);
		}
		char buffer[MAX_BUF_LEN];
		strcpy(buf,"Registering");
		int rgst_state = 0;
		while(rgst_state == 0)
		{
				int ret = send(rgst_fd,buffer,sizeof(buffer),0);
				if(ret < 0)
				{
						cout << "send err while regstering." << endl;
						exit(-1);
				}
				else
						cout << "sent message \" "<< buffer << "\"" << endl; 
				ret = recv(netinfo.get_fd(),buffer,MAX_BUF_LEN,0);
				if( strcmp(buffer,"Successfully registered"))
						rgst_state = 1;
		}
		close(rgst_fd);
		return 0;
}





		return 0;
}


