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


int Server::register_router(string router_ip,int router_port)
{
		struct sockaddr_in routeraddr;
		routeraddr.sin_family = AF_INET;
		routeraddr.sin_port = htons((short)router_port);
		routeraddr.sin_addr.s_addr = inet_addr(router_ip.c_str());
		memset(&routeraddr.sin_zero, 0, 8);

		int rgst_fd = net_info.set_fd();
		int ret = connect(rgst_fd,(struct sockaddr *)&routeraddr,sizeof(struct sockaddr));
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
		while(1)
		{
				cout << "Epoll iter begins" << endl;
				int nfds = epoll_wait(epfd,events,20,-1);
				for(int i = 0; i < nfds; ++i)
				{
						if(events[i].data.fd == fd)
						{
								cout <<"Server : Begin Accept" << endl;
								connfd = accept(listenfd,(struct sockaddr *)&clientaddr, 
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
												sdatap->fd = -1;
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
		close(listenfd);
		return 0;
}
