/**
 * @Author: Nonrustknife <chris>
 * @Date:   07-Aug-2021
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
 #include "time.h"

#define MAX_BUF_LEN 1024

 int main(int argc, char *argv[])
 {
 	//1.创建tcp套接字

 	int clientsockfd = socket(AF_INET, SOCK_STREAM, 0);
 	printf("sockfd:%d\n", clientsockfd);

 	struct sockaddr_in cli_addr;
 	socklen_t clilen = sizeof(struct sockaddr_in);
 	//2.绑定sockfd
  cli_addr.sin_family = AF_INET;
 	cli_addr.sin_port = htons(8001);
 	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 	//cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int ret = connect(clientsockfd, (struct sockaddr*)&cli_addr, sizeof(struct sockaddr));
  if(ret == -1)
  {
    printf("connect fail!\n");
    return 0;
  }
  else
  {
    printf("connect succ!\n");
  }
  char sendBuf[MAX_BUF_LEN];
  char recvBuf[MAX_BUF_LEN];

  while(1)
  {
    printf("to server:");
    bzero(sendBuf, 0);
    scanf("%[^\n]%*c", sendBuf);
    send(clientsockfd, sendBuf, strlen(sendBuf), 0);


    recv(clientsockfd, recvBuf, MAX_BUF_LEN, 0);
    printf("from server:%s\n", recvBuf);
    memset(recvBuf, 0, MAX_BUF_LEN);
  }

  return 0;
}
