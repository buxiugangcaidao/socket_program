/**
 * @Author: Nonrustknife <chris>
 * @Date:   06-Aug-2021
 * @Email:  cm656879@outlook.com
 * @Last modified by:   chris
 * @Last modified time: 10-Aug-2021
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "usp_epoll.h"


int main(int argc, char *argv[])
{
	struct epoll_event event;   // 告诉内核要监听什么事件
  struct epoll_event wait_event[MAX_EVENT_COUNT]; //内核监听完的结果

	//1.创建tcp监听套接字

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//printf("sockfd:%d\n", sockfd);

	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	//2.绑定sockfd
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(8001);
	//my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));

	//3.监听listen
	listen(sockfd, 10);

	int epfd = usp_epoll_create(10); // 创建一个 epoll 的句柄，参数要大于 0， 没有太大意义
	if( -1 == epfd )
	{
	    perror ("epoll_create");
	    return -1;
	}
	//printf("epfd:%d\n", epfd);

	event.fd = sockfd;     //监听套接字
	event.events = EPOLLIN; // 表示对应的文件描述符可以读

	//5.事件注册函数，将监听套接字描述符 sockfd 加入监听事件
	int ret = usp_epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
	if(-1 == ret)
	{
	    perror("epoll_ctl");
	    return -1;
	}
	//printf("usp_epoll_ctl succ!\n");

	int len = 0;
	char recvBuf[128] = "";
	char sendBuf[128] = "";
	//6.对已连接的客户端的数据处理
	while(1)
	{
		// 监视并等待多个文件（标准输入，udp套接字）描述符的属性变化（是否可读）
        // 没有属性变化，这个函数会阻塞，直到有变化才往下执行，这里没有设置超时
    ret = usp_epoll_wait(epfd, wait_event, MAX_EVENT_COUNT, 1000);

		//printf("[%s]ret:%d\n", __FUNCTION__, ret);

		for(int i = 0; i < ret; ++i)
		{
			for(int loop = 0; loop < ret; loop++)
			{
				//printf("wait_event[%d]:%d\n", loop, wait_event[loop].fd);
			}

			//6.1监测sockfd(监听套接字)是否存在连接
			if(( sockfd == wait_event[i].fd ) && ( EPOLLIN == (wait_event[i].events & EPOLLIN) ) )
			{
				//6.1.1 从tcp完成连接中提取客户端
				int connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
				printf("connect success!ip:%s|cli_port:%d|fd:%d\n", inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port, connfd);
				//6.1.2 将提取到的connfd放入fd数组中，以便下面轮询客户端套接字
				event.fd = connfd; //监听套接字
				event.events = EPOLLIN; // 表示对应的文件描述符可以读

				//6.1.3.事件注册函数，将监听套接字描述符 connfd 加入监听事件
				ret = usp_epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
				if(-1 == ret)
				{
					perror("epoll_ctl");
					return -1;
				}

			}

			else if(EPOLLIN == (wait_event[i].events & (EPOLLIN|EPOLLERR)))
			{
				int curfd = wait_event[i].fd;

				//6.2.1接受客户端数据
				if((len = recv(curfd, recvBuf, sizeof(recvBuf), 0)) < 0)
				{
					if(errno == ECONNRESET)//tcp连接超时、RST
					{
						close(curfd);
					}
					else
						perror("read error:");
				}
				else if(len == 0)//客户端关闭连接
				{
					printf("connect broken!\n");
					close(curfd);
				}
				else//正常接收到服务器的数据
				{
					//send(curfd, buf, len, 0);
					recvBuf[len] = '\0';
					printf("from client %d:%s\n", curfd, recvBuf);
					bzero(recvBuf, 0);
				}

				event.fd = curfd;
				event.events = EPOLLOUT;
				usp_epoll_ctl(epfd, EPOLL_CTL_MOD, curfd, &event);
			}

			if(EPOLLOUT == (wait_event[i].events & (EPOLLOUT|EPOLLERR)))
			{
				int curfd = wait_event[i].fd;
				bzero(sendBuf, 0);
				printf("to client %d:", curfd);
				scanf("%[^\n]%*c", sendBuf);
				len = strlen(sendBuf);
				printf("len:%d\n", len);

				send(curfd, sendBuf, len, 0);

				event.fd = curfd;
				event.events = EPOLLIN;
				usp_epoll_ctl(epfd, EPOLL_CTL_MOD, curfd, &event);
			}
		}
	}
	return 0;
}
