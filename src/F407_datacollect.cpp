/*
Author:LZH
TARGET:TCP服务端获取F407发过来8个通道的数据，并且解析发送到modbustcp
TIME:2023.4.21
*/
#include "../inc/main.h"

//从407获取到原始数据
uint8_t Tcp_f407_Callbackbuf[128];
//包头协议正确的F407数据
uint8_t F407_Buffer[37];
//校验的F407数据
uint8_t F407_crc[35];
//组包数组
unsigned char Data_From_F407[4];


void Tcp_sever_init(string Sever_Port){
    //数据初始化
    float *Bytetofloat = NULL;
    uint8_t *send_buf = NULL;
    uint16_t My_Port = 0;
    My_Port = atoi(Sever_Port.c_str());
    int sfd = -1, cfd = -1;
    int ret = -1, len = 0;
    uint8_t cmd_len = 0, size = 0;
    char client_ip[128] = {0};
    static int cnt;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t addr_len;
    struct timeval tv_out;
    uint16_t CRC_S;
    uint16_t origin_data,My_common_data;
    double out;
    int flags = 0;
    string Port_N = "Port0";

     //AF_INET:ipv4   SOCK_STREAM:流协议   0:默认协议(tcp,udp)
    sfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sfd != -1);
    //绑定前先构造出服务器地址
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //网络字节序
    serv_addr.sin_port = htons((atoi(Sever_Port.c_str())));
    //INADDR_ANY主机所有ip
    serv_addr.sin_addr.s_addr = INADDR_ANY;
	//serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sfd, 128);

    printf("wait for connect ...\r\n");
    addr_len = sizeof(client_addr);
    while(1){
        cfd = accept(sfd, (struct sockaddr *)&client_addr, &addr_len);
        printf("cfd = %d\r\n", cfd);
        printf("F407_IP :%s\t%d\r\n",
                        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
                        ntohs(client_addr.sin_port));
        //阻塞接受数据
        while(1){
            bzero(Tcp_f407_Callbackbuf, sizeof(Tcp_f407_Callbackbuf));
    	    len = read(cfd, Tcp_f407_Callbackbuf, sizeof(Tcp_f407_Callbackbuf));
             if(len < 0){
                perror("read error!\n");
                break;
            }
            else if(len == 0){
                perror("connect error\n");
                break;
            }
            else if(len > 2000){
                perror("tcp overflow\n");
                bzero(Tcp_f407_Callbackbuf,sizeof(Tcp_f407_Callbackbuf));
                break;
            }
            //循环遍历，寻找合适的包头
            for(int i = 0 ; i < (sizeof(Tcp_f407_Callbackbuf) - 21) ; i++){
                //包头正确
                if(Tcp_f407_Callbackbuf[i] == 0x01 && Tcp_f407_Callbackbuf[i + 1] == 0x04 && Tcp_f407_Callbackbuf[i + 2] == 0x10 ){
                    //将合适的数据放到另外一个数组
                    for(int j = 0 ; j < sizeof(F407_Buffer) ; j++){
                        F407_Buffer[j] = Tcp_f407_Callbackbuf[i+j];
                    }
                    //将需要进行校验的数据放到校验数组中
                    for(int k = 0 ; k < sizeof(F407_crc) ; k++){
                        F407_crc[k] = F407_Buffer[k];
                    } 
                    //进行校验
                    CRC_S = getModbusCRC16(F407_crc,sizeof(F407_crc));
                    printf("crc:%02x\n",CRC_S);
                    printf("end:%02x\n",(F407_Buffer[36] << 8 | F407_Buffer[35]));
                    uint8_t high = (CRC_S >> 8) & 0xff;
	                uint8_t low = (CRC_S) & 0xff;
                    printf("high:%x\n",high);
                    printf("low:%x\n",low);
                    if(CRC_S == (F407_Buffer[36] << 8 | F407_Buffer[35])){
                      //校验成功
                      for(int n = 0 ; n < atoi(Factory_map["IP_NUM"].c_str()) ; n++){
                        //遍历全部当前车间的全部IP
                        string::reverse_iterator it = Port_N.rbegin();
                        *it = (( n + 1 ) + '0');
                        if(Factory_map[Port_N].compare(Sever_Port) == 0){ 
                                //当前Port与当前线程的Port相匹配
                                for(int h = 0 ; h < atoi(Factory_map[("Channel"+to_string(n+1))].c_str()) ; h++ ){
                                    //数据处理
                                    //01 04 10 C8 B4 40 0E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0B 49 
                                    Data_From_F407[0] = Tcp_f407_Callbackbuf[3+(h*4+1)];
                                    Data_From_F407[1] = Tcp_f407_Callbackbuf[3+(h*4+0)]; 
                                    Data_From_F407[2] = Tcp_f407_Callbackbuf[3+(h*4+3)];
                                    Data_From_F407[3] = Tcp_f407_Callbackbuf[3+(h*4+2)];
                                    Bytetofloat = (float*)Data_From_F407;
                                    printf("Byte:%g\n",*Bytetofloat);
                                    // origin_data = ((Tcp_f407_Callbackbuf[(2*h)+3] << 8) | Tcp_f407_Callbackbuf[(2*h)+4]);
                                    // out = (double)origin_data;
                                    //第一次往map里面插入数据，第二次往后更改各给车间IP的对应Value
                                    switch(flags){

                                        case 0:Factory_data_map.insert(pair<string,double>((("IP"+to_string(n+1))+("Channel"+to_string(h+1))),*Bytetofloat));flags=1;break;

                                        case 1:Factory_data_map[(("IP"+to_string(n+1))+("Channel"+to_string(h+1)))] = *Bytetofloat;break;

                                        default:break;
                                    }
                                    //清空指针和数组
                                    Bytetofloat = NULL;
                                    bzero(Data_From_F407,sizeof(Data_From_F407));
                                }
                        }
                      }
                    }
                    break;
                }
            }
        }
        sleep(1);
        close(cfd);
        cfd = -1;
    }
    close(sfd); //
}

//Modbuscrc_16校验
static unsigned short int getModbusCRC16(unsigned char *_pBuf, unsigned short int _usLen)
{
    unsigned short int CRCValue = 0xFFFF;           //初始化CRC变量各位为1
    unsigned char i,j;

    for(i=0;i<_usLen;++i)
    {
        CRCValue  ^= *(_pBuf+i);                    //当前数据异或CRC低字节
        for(j=0;j<8;++j)                            //一个字节重复右移8次
        {
            if((CRCValue & 0x01) == 0x01)           //判断右移前最低位是否为1
            {
                 CRCValue = (CRCValue >> 1)^0xA001; //如果为1则右移并异或表达式
            }else 
            {
                CRCValue >>= 1;                     //否则直接右移一位
            }           
        }
    } 
    return CRCValue;            
}



string string_to_hex(const string& str) //transfer string to hex-string
{
    string result="0x";
    string tmp;
    stringstream ss;
    for(int i=0;i<str.size();i++)
    {
        ss<<hex<<int(str[i])<<endl;
        ss>>tmp;
        result+=tmp;
    }
    return result;
}

