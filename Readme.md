从F407 8266的TCP功能读取F407各个通道的模拟输入的值

详情配置请看./bin/Factory_config.json

解析json文件:

            1.Factory_x 代表的是第几个车间 IP_NUM代表这个车间有多少个IP(线程)，Channel_num代表这个线程使用了多少个通道
            2.若需要添加F407(子板)，则需要在相应的车间添加对应的IP和端口号，程序会自动适配添加相应的线程
            3.通过配置文件实现偏差值校正功能目前还在开发中
            4.若修改了配置文件，则需要重启程序
