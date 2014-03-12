#include<cstdio>
#include<cstdlib>
#include<iostream>
#include"network.h"

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
		strcpy(buffer,"Server Register");
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
		if(get_mq(fd) == "Client Request")
				set_mq(fd,"Server Reponse");
}
void Router::work(int fd)
{
		if(get_mq(fd) == "Server Register")
				set_mq(fd,"Successfully Registered");
		if(get_mq(fd) == "Routing")
				set_mq(fd,"Routing Finished");
}

int NetInfo::process_request(class NetInfo &serve_node)
{
		cout << "Server is starting to work" << endl;
		int connfd,sockfd;
		struct sockaddr_in clientaddr;
		socklen_t clientaddr_len;
		char line[MAXLINE];
		int n;
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
								//write(sockfd, line, n);
								const char *ptr = get_mq(sockfd).c_str();
								send(sockfd,ptr,strlen(ptr)+1,0);
								printf("Send %s finish\n",line);
								ev.data.fd =sockfd;
								ev.events = EPOLLIN|EPOLLET;
								epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
								printf("EPOLLOUT End\n");
						}	
						
				}
		}
		close(fd);
		return 0;
}
