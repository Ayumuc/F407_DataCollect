/*
AUTHOR:LZH
TARGET:建立tcp服务端，并且将RS485和客户端获取到的数据发到plcgetway
*/
#include "../inc/main.h"

#define SERV_PORT     8268
#define READ_CMD_LEN  12

uint8_t buf[2048];

void Tcpsever_init();

void Tcpsever_init(){
	
    uint8_t *send_buf = NULL;
    int sfd = -1, cfd = -1;
    int ret = -1, len = 0;
    uint8_t cmd_len = 0, size = 0;
    char client_ip[128] = {0};
    static int cnt;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t addr_len;
    struct timeval tv_out;

     //AF_INET:ipv4   SOCK_STREAM:流协议   0:默认协议(tcp,udp)
    sfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sfd != -1);
    //绑定前先构造出服务器地址
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //网络字节序
    serv_addr.sin_port = htons(SERV_PORT);
    //INADDR_ANY主机所有ip
    serv_addr.sin_addr.s_addr = INADDR_ANY;
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    //服务器能接收并发链接的能力
    listen(sfd, 128);

    printf("modbus wait for connect ...\r\n");
    addr_len = sizeof(client_addr);
    while (1)
    {
    	 //阻塞，等待客户端链接，成功则返回新的文件描述符，用于和客户端通信
    	    cfd = accept(sfd, (struct sockaddr *)&client_addr, &addr_len);
    	    printf("cfd = %d\r\n", cfd);


    	    printf("client IP:%s\t%d\r\n",
    	      	            inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
    	      	            ntohs(client_addr.sin_port));
    	    tv_out.tv_sec = 10;
			tv_out.tv_usec = 0;
			ret = setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
			printf("tv_sec = %d tv_usec = %d\r\n", (int)tv_out.tv_sec, (int)tv_out.tv_usec);
			if (ret == -1)
			{
				printf("setsockopt errno = %d\r\n", errno);
			}

			while (1)
    	    {
				if (cnt++ > 10000)
				{
					cnt = 0;
				}
    	        //阻塞接收客户端数据
				bzero(buf, sizeof(buf));
    	        len = read(cfd, buf, sizeof(buf));
    	        if (len == 0)
    	        {
    	        	printf("connect close\r\n");
    	        	break;
    	        }

    	        else if (len < 0)
    	        {
    	    	    printf("recv fail, timeout\r\n");
    	        	break;
    	        }

    	        else if (len > 2000)
    	        {

    	    	    printf("tcpbuf overflow\r\n");
    	        	bzero(buf, sizeof(buf));
    	        	break;
    	        }

    	        else   //成功接收到数据
    	        {	
    	        	if (buf[2] != 0 && buf[3] != 0 ) //协议标识
    	        	{
    	    	    printf("Protocol identifiers\r\n");
    	        		break;
    	        	}
    	        	cmd_len = (buf[4] << 8 | buf[5]) + 6;
    	        	if ( cmd_len != READ_CMD_LEN) //命令长度
    	        	{
    	    	    printf("cmd length error\r\n");
    	        		break;
    	        	}

    	        	send_buf = parseModbusCommand(buf, cmd_len, &size);
//    	        	printf("size = %d\r\n",  size);
					// printf("2\n");
    	            ret = send(cfd, send_buf, size, 0);
					// printf("1\n");
    	            if (ret < 0)
    	            {
    	    	    printf("Error occured during sending\r\n");
    	                break;
    	            }
    	        }
				usleep(100*10);
    	    }
			sleep(1);
    	    close(cfd);
    	    cfd = -1;
    }
    close(sfd); //
    pthread_exit(NULL);
}