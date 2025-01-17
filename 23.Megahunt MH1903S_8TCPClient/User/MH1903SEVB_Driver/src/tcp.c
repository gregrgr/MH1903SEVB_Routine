#include "tcp.h"
#include "delay.h"

uint8 buffer[2048];												// 定义一个2KB的数组，用来存放Socket的通信数据
uint8 server_ip1[4]={192,168,1,20};			// 配置远程服务器IP地址
uint8 server_ip2[4]={192,168,1,21};			// 配置远程服务器IP地址
uint16 server_port1=5000;								// 配置远程服务器端口
uint16 server_port2=6000;								// 配置远程服务器端口
uint16 server_port3=7000;								// 配置远程服务器端口
uint16 server_port4=8000;								// 配置远程服务器端口

uint16 local_port = 5000;												// 定义一个本地端口并初始化
uint16 len = 0;

void tcpc(uint8 i)
{
	switch (getSn_SR(i))														// 获取socket0的状态
	{
	case SOCK_INIT:															// Socket处于初始化完成(打开)状态
		//printf("Socket%d-1 ", i);
		Delay_ms(500);
		if (i == 0)
		{	connect(i, server_ip1, server_port1);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 1)
		{	connect(i, server_ip1, server_port2);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 2)
		{	connect(i, server_ip1, server_port3);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 3)
		{	connect(i, server_ip1, server_port4);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 4)
		{	connect(i, server_ip2, server_port1);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 5)
		{	connect(i, server_ip2, server_port2);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 6)
		{	connect(i, server_ip2, server_port3);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
		else if(i == 7)
		{	connect(i, server_ip2, server_port4);	}		// 配置Sn_CR为CONNECT，并向TCP服务器发出连�
		
		break;
		
	case SOCK_ESTABLISHED:											// Socket处于连接建立状态
		//printf("Socket%d-2 ", i);
		Delay_ms(500);
		if (getSn_IR(i) & Sn_IR_CON)
		{
			setSn_IR(i, Sn_IR_CON);								// Sn_IR的CON位置1，通知W5500连接已建立
			printf("%d:connected.\r\n", i);
		}
		// 数据回环测试程序：数据从上位机服务器发给W5500，W5500接收到数据后再回给服务器
		len = getSn_RX_RSR(i);										// len=Socket0接收缓存中已接收和保存的数据大小
		if (len > 0)
		{
			recv(i, buffer, len);										// W5500接收来自服务器的数据，并通过SPI发送给MCU
			printf("%d:%s\r\n", i, buffer);							// 串口打印接收到的数据
      send(i,buffer,len);										// 接收到数据后再回给服务器，完成数据回环
			memset(buffer,0,len+1);
		}
		// W5500从串口发数据给客户端程序，数据需以回车结束
		if (USART_RX_STA & 0x8000)								// 判断串口数据是否接收完成
		{
			len = USART_RX_STA & 0x3fff;						// 获取串口接收到数据的长度
			send(i, USART_RX_BUF, len);							// W5500向客户端发送数据
			USART_RX_STA = 0;												// 串口接收状态标志位清0
			memset(USART_RX_BUF, 0, len + 1);					// 串口接收缓存清0
		}
		break;
		
	case SOCK_CLOSE_WAIT:												// Socket处于等待关闭状态
		//printf("Socket%d-3 ", i);
		Delay_ms(500);
		len = getSn_RX_RSR(i);										// len=Socket0接收缓存中已接收和保存的数据大小
		if (len > 0)
		{
			recv(i, buffer, len);										// W5500接收来自服务器的数据，并通过SPI发送给MCU
			printf("%d:%s\r\n", i, buffer);							// 串口打印接收到的数据
			send(i, buffer, len);										// 接收到数据后再回给服务器，完成数据回环
		}
		disconnect(i);																// 断开Socket0的连接
		break;
		
	case SOCK_CLOSED:														// Socket处于关闭状态
		//printf("Socket%d-4 ", i);
		Delay_ms(500);
		socket(i, Sn_MR_TCP, local_port++, Sn_MR_ND);		// 打开Socket0，并配置为TCP无延时模式，打开一个本地端口
		break;
	}
}

void do_8tcpc(void)
{
	int i = 0;
	for (i = 0; i <= 7; i++)
	{
		tcpc(i);
	}
}
