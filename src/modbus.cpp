/*
Author:LZH
TARGET:modbus协议解析和组包，并且将浮点数转换为CDAB输出
*/
#include "../inc/main.h"
#include "../inc/Acceleration.h"
#include "../inc/data_collect.h"
#define READ_DISCRETE_INPUTS       2
#define READ_HOLDING_REGISTERS     3
#define WRITE_SINGLE_COIL          5
#define WRITE_MULTIPLE_REGISTERS   15
#define KEY_START_ADDRESS 1
#define KEY_STOP_ADDRESS 3
unsigned char* getAnalogValueBytes(void);
void floatToByte(float f,unsigned char *byte);
enum KY{
	OFF = 0,
	ON,
};




float x_value, y_value;
double slave2_value_ch1;
double slave2_value_ch2;
double slave3_value_ch1;
double slave3_value_ch2;
double slave4_value_ch1;
double slave4_value_ch2;
double slave5_value_ch1;
double slave5_value_ch2;
double analogValues[18];

double  FloatData[10];
uint8_t Bytes[40];


unsigned char* getFloatBytes(void)
{
	int8_t i;
	for(i=0; i<10; i++)
	{
		floatToByte(FloatData[i], &Bytes[i*4]);
	}
	return Bytes;
}



//功能码0x03
// modbustcp地址：  0 -- A10 地址
//				  1 -- 苏州碰碰车开键位检测
//                2 -- A11 地址
//				  3 -- 苏州碰碰车关键位检测
//				  4 -- A20 地址
//				  6 -- A21 地址
//				  8 -- A30 地址
//				  10 -- A31 地址
//				  12 -- A40 地址
//				  14 -- A41 地址

//                16 -- 角度传感器X轴数据
//                18 -- 角度传感器y轴数据
//                20 -- addr = 2 的板子的ch1 数据
//                22 -- addr = 2 的板子的ch2 数据
//                24 -- addr = 3 的板子的ch1 数据
//                26 -- addr = 3 的板子的ch2 数据
//                28 -- addr = 4 的板子的ch1 数据
//                30 -- addr = 5 的板子的ch2 数据
//                32 -- addr = 5 的板子的ch1 数据
//                34 -- addr = 5 的板子的ch2 数据

uint8_t parseResult[100]={0};

uint8_t* parseModbusCommand(uint8_t *modbusCmd, int32_t comLength, uint8_t* size)//解析modbusTCP数据
{
	char     slaveId = 0;
	int32_t  Address = 0;
	int32_t  Length = 0;
	int32_t  commandlenght=0;
	int32_t  dataLength = 0;
	uint8_t* analogValueBytes;
	int32_t  i=0;

	*size = 0;

	if((modbusCmd[2]!=0)||(modbusCmd[3]!=0))
	{
		return 0;//protocol type
	}

	commandlenght = modbusCmd[4] << 8 | modbusCmd[5];
	if(commandlenght != (comLength-6))
	{
		return 0;
	}

	slaveId = modbusCmd[6];

	Address = modbusCmd[8] << 8 | modbusCmd[9];
	Length = modbusCmd[10] << 8 | modbusCmd[11];//在写线圈的时候，这代表高或者低
	if(Address < 0 || Length < 0)
	{
		return 0;
	}
	if(Address == KEY_START_ADDRESS){
		printf("start!\r\n");
	}
	else if(Address == KEY_STOP_ADDRESS){
		printf("stop!\r\n");
	}
	parseResult[0] = modbusCmd[0];
	parseResult[1] = modbusCmd[1];
	parseResult[2] = 0;
	parseResult[3] = 0;
	parseResult[6] = slaveId;

	switch(modbusCmd[7])//function code
	{
		case WRITE_SINGLE_COIL:
			if(Address > 3)
			{
				return 0;
			}
			parseResult[4] = modbusCmd[4];
			parseResult[5] = modbusCmd[5];
			parseResult[7] = WRITE_SINGLE_COIL;
			parseResult[8] = modbusCmd[8];
			parseResult[9] = modbusCmd[9];
			parseResult[10] = modbusCmd[10];
			parseResult[11] = modbusCmd[11];
			*size = 12;
			if(parseResult[10] == 0xFF)
			{
				//writeDigitOutputStatus(Address,0x00);
			}
			else
			{
				//writeDigitOutputStatus(Address,0x01);
			}
			break;

		case READ_DISCRETE_INPUTS:
			if(Address + Length > 4)return 0;
			parseResult[8] = 1;
			dataLength = 3 + parseResult[8];
			parseResult[4] = dataLength >> 8;
			parseResult[5] = dataLength & 0xff;
			parseResult[7] = READ_DISCRETE_INPUTS;
			*size = dataLength + 6;
			parseResult[9] = 0;
			for(i=0; i<Length; i++)
			{
				//parseResult[9] |= readDigitInputStatus(Address+i) << (i);
			}
			break;

		case READ_HOLDING_REGISTERS:
//			printf("address = %d\r\n", Address);


			if (Length > 36)
			{
				Length = 36; //只能读取36个保持寄存器
				printf("modbus tcp read holding register length more than 36!\r\n");
			}

			analogValues[8] = x_value; //角度传感器x轴数据
			analogValues[9] = y_value; //角度传感器x轴数据
			analogValues[10] = slave2_value_ch1;       //addr = 1 的板子的ch1 数据
			analogValues[11] = slave2_value_ch2;       //addr = 1 的板子的ch2 数据
			analogValues[12] = slave3_value_ch1;       //addr = 2 的板子的ch1 数据
			analogValues[13] = slave3_value_ch2;       //addr = 2 的板子的ch2 数据
			analogValues[14] = slave4_value_ch1;       //addr = 3 的板子的ch1 数据
			analogValues[15] = slave4_value_ch2;       //addr = 3 的板子的ch2 数据
			analogValues[16] = slave5_value_ch1;       //addr = 4 的板子的ch1 数据
			analogValues[17] = slave5_value_ch2;       //addr = 4 的板子的ch2 数据

			parseResult[8] = Length*2;
			dataLength = 3 + parseResult[8];
			parseResult[4] = dataLength >> 8;
			parseResult[5] = dataLength & 0xff;
			parseResult[7] = READ_HOLDING_REGISTERS;
			*size = dataLength + 6;
			analogValueBytes = getAnalogValueBytes();

			for(i=0; i<Length; i++)
			{
				parseResult[9+i*2]   = analogValueBytes[Address*2+ i*2];
				parseResult[9+i*2+1] = analogValueBytes[Address*2 + i*2+1];
			}
			break;

		default:
			return 0;
			break;
	}
//	printf("analogValueBytes:");
//		for(int i = 0 ; i < 84 ; i++){
//			printf("%02x ",analogValueBytes[i]);
//		}
//	printf("\r\n");
//	printf("parseResult:\r\n");
//	for(int i = 0 ; i < sizeof(parseResult) ; i++){
//		printf("%02x ",parseResult[i]);
//	}
//	printf("\r\n");

	return parseResult;
}


unsigned char analogValueBytes[72];


unsigned char* getAnalogValueBytes(void)
{
	double analogValues[20];
	int count = 0;
	for(int n = 0 ; n < atoi(Factory_map["IP_NUM"].c_str()) ; n++){
		for(int k = 0 ; k < atoi(Factory_map[("Channel"+to_string(n+1))].c_str()) ; k++){
			analogValues[count] =  Factory_data_map[("IP"+to_string(n+1))+("Channel"+to_string(k+1))];
			count++;
		}
	}
	printf("\n");
	for(int n = 0 ; n < 6 ; n++){
		printf("analog[%d]:%f",n,analogValues[n]);
	}
	// for(auto iy = Factory_data_map.begin(); iy != Factory_data_map.end() ; iy++){
	// 	cout << "key:" << iy->first <<"   value:" << iy->second << endl;
	// }
	char* write_buffer = NULL; 
	int8_t i;
	for(i=0;i<18;i++)
	{
		floatToByte(analogValues[i], &analogValueBytes[i*4]);
	}
	return analogValueBytes;
}

void floatToByte(float f,unsigned char *byte)
{
	unsigned char *pdata;
	pdata = (unsigned char*)&f;
	byte[1] = *pdata++;
	byte[0] = *pdata++;
	byte[3] = *pdata++;
	byte[2] = *pdata;
}


