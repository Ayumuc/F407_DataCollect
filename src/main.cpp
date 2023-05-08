#include "../inc/main.h"
#include "../inc/Acceleration.h"
#include "../inc/data_collect.h"
int data_pos;
char speed_value[4];
char Ax_Handle_buffer[50];
char Ay_Handle_buffer[50];
char Az_Handle_buffer[50];
Common_Data common_data;
Acler Aer2;
Acler Aer;
My_Factory my_Factory;
map<string,string> Factory_map;
map<string,double> Factory_data_map;
int main(int argc , char**argv){
    //读取配置文件
    config_read();
    pthread_t thread_id;
    pthread_t usart_id;
    vector<thread> My_vector;
    string Port_N = "Port0";
    //循环一共有多少个IP
    for(int i = 0 ; i < atoi(Factory_map["IP_NUM"].c_str()) ; i++){
        //反向迭代器获取string最后一个字节
        string::reverse_iterator it = Port_N.rbegin();
        //迭代器加一，相当于map指向下一个IP
        *it = (( i + 1 ) + '0');
        //把当前IP的PORT作为参数传进线程中，通过vector创建线程
        My_vector.push_back(thread(Tcp_sever_init,Factory_map[Port_N]));
        //延时一下以免线程冲突
        usleep(100*10);
    }
    //创建modbus线程
    My_vector.push_back(thread(Tcpsever_init));
    for(auto& i:My_vector){
        //避免僵尸线程
        i.join();
    }
    while (1){
        sleep(1);
    }
}




void config_read(void){
    std::ifstream ifs;
	
	ifs.open("Factory_config.json");
	
	assert(ifs.is_open());
	
	Json::Reader reader;
	Json::Value root;
    std::string F_Num_before = "Factory_";
	char F_Num_after = 'A';
    std::string F_Sum = "";
    std::string out = "";
    int ip_num = 0;
    if (!reader.parse(ifs, root, false))
	{
		std::cout<<"reader.parse() fail"<<std::endl;
		return ;
	}
    
    for(int i = 0 ; i < 6 ; i++){
        F_Num_after = 'A'+i;
        F_Sum = F_Num_before + F_Num_after ;
        const Json::Value Array = root[F_Sum];
        out = Array[0]["Key"].asString();
        if(out.compare("True") == 0){
            out = Array[1]["IP_NUM"].asString();
            Factory_map.insert(pair<string,string>("IP_NUM",out));
            ip_num = atoi(out.c_str());
            for(int j = 0 ; j < ip_num ; j++){
                out = Array[j+2]["IP"].asString();
                Factory_map.insert(pair<string,string>(("IP"+to_string(j+1)),out));
                out = Array[j+2]["Port"].asString();
                Factory_map.insert(pair<string,string>(("Port"+to_string(j+1)),out));
                out = Array[j+2]["Channel_Num"].asString();
                Factory_map.insert(pair<string,string>(("Channel"+to_string(j+1)),out));
                for(int n = 0 ; n < atoi(Factory_map["Channel"+to_string(j+1)].c_str()) ; n++){
                    out = Array[j+2]["Channel"+to_string(n+1)+"_offset"].asString();
                    Factory_map.insert(pair<string,string>(("Channel"+to_string(n+1)+"offset"),out));
                }
            }
            for(auto it = Factory_map.begin(); it != Factory_map.end() ; it++){
                cout << "key:" << it->first <<"   value:" << it->second << endl;
            }
            break;
        }    
    }
    return ;
}