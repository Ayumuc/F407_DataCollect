
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
 
int main(void)
{
    int ret = -1;
    // 1、使用socket()函数获取一个socket文件描述符
	int tcp_server = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == tcp_server)
	{
		perror("socket");
		return -1;
	}
 
    // 2. 绑定本地的相关信息，如果不绑定，则系统会随机分配一个端口号
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;		                    // 设置地址族为IPv4
	local_addr.sin_port = htons(8266);	                        // 设置地址的端口号信息
	local_addr.sin_addr.s_addr = INADDR_ANY;	//　设置IP地址
	ret = bind(tcp_server, (const struct sockaddr *)&local_addr, sizeof(local_addr));
	if (ret < 0)
		perror("bind");
    else	
        printf("bind success.\n");
 
    // 3、listen监听端口，阻塞，等待客户端来连接服务器
	ret = listen(tcp_server, 5);
 
    // 4、accept阻塞等待客户端接入
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
	int client_fd = accept(tcp_server, (struct sockaddr *)&client_addr, &addrlen);
 
    // 5、等待接收对方发送过来的数据
    char recvbuf[1024] = {0};
    ret = recv(client_fd, recvbuf, sizeof(recvbuf), 0);
    printf("Recv from %s:%d, ",inet_ntoa(*(struct in_addr*)&client_addr.sin_addr.s_addr),ntohs(client_addr.sin_port));
    printf("recv_data: %s \n",recvbuf);
 
    // 6、返回数据到客户端
    char send_buf[1024] = "I have got the date:";
    strcat(send_buf,recvbuf);
    send(client_fd, send_buf, strlen(send_buf), 0);
 
     // 7、关闭套接字
    close(tcp_server);
}