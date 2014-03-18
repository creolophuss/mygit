#include<cstdio>
#include<iostream>
#include<boost/regex.hpp>
#include"network.h"

void rm_ch(char *str, char c)
{
		char *p,*q;
		p = q = str;
		while(*q != '\0')
		{
				if( *p != c  )
						p++;
				q++;
				while( *q == c )
						q++;
				*p = *q;
		}
}	

static int
make_socket_non_blocking(int sfd)
{
		int flags , s;
		flags = fcntl(sfd,F_GETFL);
		if(flags == -1)
		{
				perror("fcntl err");
				return -1;
		}
		flags |= O_NONBLOCK;
		s = fcntl(sfd, F_SETFL, flags);
		if(s == -1)
		{
				perror("fcntl err");
				return -1;
		}

		return 0;
}
int NetInfo::get_fd()
{
		return fd;
}
int NetInfo::set_fd()
{
		fd = socket(PF_INET,SOCK_STREAM,0);
		return fd;
}
int NetInfo::set_epfd()
{
		epfd = epoll_create(256);
		return epfd;
}
int NetInfo::init_ev()
{
		cout << "Init ev : " << ip << " : " << port << endl;
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

		cout <<"fd == " << fd << endl;
		return 0;
}
string NetInfo::get_mq(int fd)
{
		return this->msgq[fd];
}

void NetInfo::set_mq(int fd,string str)
{
		this->msgq[fd] = str;
}


void Server::register_router(string router_ip,int router_port)
{
		cout << "router ip :" << router_ip.c_str() << endl;
		struct sockaddr_in routeraddr;
		routeraddr.sin_family = AF_INET;
		routeraddr.sin_port = htons((short)router_port);
		routeraddr.sin_addr.s_addr = inet_addr(router_ip.c_str());
		memset(&routeraddr.sin_zero, 0, 8);

		int rgst_fd = set_fd();
		int ret = connect(rgst_fd,(struct sockaddr *)&routeraddr,sizeof(struct sockaddr));
		if(ret != 0)
		{
				cout << "connect error while registering. " << endl;
				exit(-1);
		}
		char buffer[MAX_BUF_LEN];
		string rgst_request = "Server:Register:" + ip + ":" +  port_s;
		strcpy(buffer,rgst_request.c_str());
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
				ret = recv(get_fd(),buffer,MAX_BUF_LEN,0);
				if( strcmp(buffer,"Successfully Registered") == 0)
				{
						rgst_state = 1;
						cout << "Received " << buffer << endl;
						break;
				}
		}
		close(rgst_fd);
	//	return 0;
}
void Server::work(int fd)
{
		cout << "This is Server" << endl; 

		if(get_mq(fd) == "Client Request")
				set_mq(fd,"Server Reponse");
		else if(get_mq(fd) == "Sucessfully Registered")
				cout << " Registered" << endl;
		else
				set_mq(fd,"Server Reponse");

}

void Router::work(int fd)
{
		cout << "This is Router" << endl;
		using namespace::boost;
		smatch m;
		regex client_re("(Client):(Search):(.+)");
		regex server_re("(Server):(Register):(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+)");
		string str = get_mq(fd);

		if(regex_match(str,m,client_re))
		{
				cout << "Processing Client Request " << endl;
				vector<string> &v = route(m[3].str());
				string response;
				for(int i = 0; i < v.size(); i++)
				{
						response += (v[i] + " ");
				}
				set_mq(fd,response);
		}
		else if(regex_match(str,m,server_re))
		{
				
				bool ret = add_new_server(m[3].str(),m[4].str());
				if(ret)
						set_mq(fd,"Successfully Registered");
		}
				
}
key_type Router::server_hashing(string ip,string port)
{
		char buffer[64];
		strcpy(buffer,ip.c_str());
		rm_ch(buffer,'.');
		cout << "Buffer : " << buffer << endl;
		int a = atoi(buffer);
		int b = atoi(port.c_str());
		key_type key = a * b % SPACE_SIZE;
		return key;
}

key_type Router::client_hashing(string str)
{
		char buffer[64];
		const char *p = str.c_str();
		key_type key= 1;
		cout << "client hashing " << endl;
		while(*p != '\0')
		{
				cout << *p ;
				key *= (int)(*p);
				key %= SPACE_SIZE;
				p++;
		}
		cout << endl;
		cout << "client key : " << key <<endl;
		return key;
}

vector<string> Router::route(string input_key)
{
		key_type key = client_hashing(input_key);
		multimap<key_type,string>::iterator it;
		int found = 0;
		set<string> s;
		set<string>::iterator s_it;
		vector<string> v;
		
		it = key_map.lower_bound(key);
		while(found != n_connections)
		{
				if(it == key_map.end())
				{
						it = key_map.begin();
				}
				else
				{
						s_it = s.find(it->second);
						if(s_it != s.end())
								it++;
						else
						{
								v.push_back(it->second);
								found++;
						}
				}
				
		}

		return v;
}

bool Router::add_new_server(string s_ip,string s_port)
{
		key_type key = server_hashing(s_ip,s_port);
		string s_info = s_ip + ":" + s_port;

		for(int i = 1; i != n_vnodes; i++)
		{
				key = key*key;
				key = (key * i) % SPACE_SIZE;
				key_map.insert(make_pair(key,s_info));
		}
		
		return true;
}

void Client::login(string r_ip,string r_port)
{
		int port = atoi(r_port.c_str());
		r_addr.sin_family = AF_INET;
		r_addr.sin_port = htons((short)port);
		r_addr.sin_addr.s_addr = inet_addr(r_ip.c_str());
		//bzero(&(r_addr.sin_zero), 8);/* in string.h */
		memset(&r_addr.sin_zero, 0, 8);
}
string Client::route_request(string key)
{
		int sockfd = socket(PF_INET,SOCK_STREAM,0);
		int ret = connect(sockfd,(struct sockaddr *)&r_addr,sizeof(struct sockaddr));
		string rqst = "Client:Search:"+key;

		char buffer[MAX_BUF_LEN];
		strcpy(buffer,rqst.c_str());
		ret = send(sockfd,buffer,sizeof(buffer),0);
		if(ret > 0)
		{
				printf("send command \" %s \" to %s:%d\n",buffer,(char*)inet_ntoa(r_addr.sin_addr),ntohs(r_addr.sin_port));
		}
		ret = recv(sockfd,buffer,MAX_BUF_LEN,0);
		if(ret > 0)
		{		
				printf("Server reply:%s\n",buffer);
		}
		return string(buffer);

}
string Client::service_request(string s_info,string rqst)
{
		using namespace::boost;
		string split = "((.+) )+"
		string pattern = "(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+)";
		regex re(pattern);
		smatch m;
		regex_match(s_info,m,re);

		struct sockaddr_in s_addr;
		int port = atoi(m[2].str().c_str());
		s_addr.sin_family = AF_INET;
		s_addr.sin_port = htons((short)port);
		s_addr.sin_addr.s_addr = inet_addr(m[1].str().c_str());
		//bzero(&(s_addr.sin_zero), 8);/* in string.h */
		memset(&s_addr.sin_zero, 0, 8);

		int sockfd = socket(PF_INET,SOCK_STREAM,0);
		cout << "Client Connect : " << m[1].str() << " : " << port << endl;
		int ret = connect(sockfd,(struct sockaddr *)&s_addr,sizeof(struct sockaddr));
		if(ret != 0)
		{
				cout << "Client can not connect !" << endl;
				exit(-1);
		}


		char buffer[MAX_BUF_LEN];
		strcpy(buffer,rqst.c_str());
		ret = send(sockfd,buffer,sizeof(buffer),0);
		if(ret > 0)
		{
				printf("send command \" %s \" to %s:%d\n",buffer,(char*)inet_ntoa(s_addr.sin_addr),ntohs(s_addr.sin_port));
		}
		ret = recv(sockfd,buffer,MAX_BUF_LEN,0);
		if(ret > 0)
		{		
				printf("Server reply:%s\n",buffer);
		}
		return string(buffer);
}

int NetInfo::process_request(class NetInfo &serve_node)
{
		cout << "Server is starting to work" << endl;
		int connfd,sockfd;
		struct sockaddr_in clientaddr;
		socklen_t clientaddr_len;
		char line[MAXLINE];
		int n;
		cout << "Listen fd == " << fd << endl;
		while(1)
		{
				cout << "Epoll iter begins" << endl;
				int nfds = epoll_wait(epfd,events,20,-1);
				for(int i = 0; i < nfds; ++i)
				{
						if(events[i].data.fd == fd)
						{
								cout <<"Server : Begin Accept" << endl;
								connfd = accept(fd,(struct sockaddr *)&clientaddr, 
												&clientaddr_len);
								if(connfd < 0){					
									   	perror("connfd<0");
										exit(1);
						  		}
								make_socket_non_blocking(connfd);  
								char *str = inet_ntoa(clientaddr.sin_addr);
								printf("Accept a connection from %s\n",str);

								ev.data.fd = connfd;
								ev.events = EPOLLIN|EPOLLET;										
								epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);

								printf("Finish Accept %d\n",connfd);
						}
						else if(events[i].events & EPOLLIN)
						{
								printf("EPOLLIN\n");

								sockfd = ev.data.fd;

								if ( (n = read(sockfd, line, MAXLINE)) < 0) 
								{
										printf("read < n\n");
										if (errno == ECONNRESET) 
										{																			
											   	close(sockfd);
										} else								
												printf("readline error.\n");
										close(sockfd);									
										continue;
							  	}
								else if(n == 0)
								{
										printf("n == 0\n");
					   					ev.events = EPOLLIN|EPOLLET;
				                		epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
										printf("EPOLLIN END\n");
										continue;
								}
								printf("Read %d characters\n",n);
								line[n] = '\0';							
								set_mq(sockfd,line);
								cout << "get_mq : " << get_mq(sockfd) << endl;

								serve_node.work(sockfd);
								
				   				ev.data.fd = sockfd;
								ev.events = EPOLLOUT|EPOLLET;
				                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);

								printf("EPOLLIN End\n");
						}
			            else if(events[i].events & EPOLLOUT)
						{
								sockfd = events[i].data.fd;

								printf("After copy\n");
								const char *ptr = get_mq(sockfd).c_str();
								send(sockfd,ptr,strlen(ptr)+1,0);
								printf("Send %s finish\n",ptr);
								ev.data.fd =sockfd;
								ev.events = EPOLLIN|EPOLLET;
								epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&ev);
								close(sockfd);
								printf("EPOLLOUT End\n");
						}	
						
				}
		}
		close(fd);
		return 0;
}
