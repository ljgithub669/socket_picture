#include <stdio.h>
#include <winsock2.h>
#include <winsock2.h>
#define SERVER_PORT 9210 //侦听端口
#pragma comment(lib,"ws2_32.lib")
 
struct Data //数据包
{
	int length;
	char receivemessage[2000]; //内容信息
	int fin;
}data;
 
int main()
{
	FILE * fp;
	if (!(fp = fopen("1.jpg", "wb+")))
	{
		printf("open 1.jpg error");
		exit(0);
	}
	WORD wVersionRequested;//双字节；高字节存取副版本号，低字节存取高版本号。
	WSADATA wsaData;//存取套接字实现的具体信息
	int ret,  length;
	SOCKET sListen, sServer; //侦听套接字，连接套接字
	struct sockaddr_in saServer, saClient; //地址信息  
 
	//WinSock初始化
	wVersionRequested = MAKEWORD(2, 2); 
	ret = WSAStartup(wVersionRequested, &wsaData); 
	if (ret != 0)
	{
		printf("WSAStartup() failed!\n");
		return 0;
	}
	//创建Socket,使用TCP协议
	sListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sListen == INVALID_SOCKET)
	{
		WSACleanup();
		printf("socket() faild!\n");
		return 0;
	}
	//构建本地地址信息
	saServer.sin_family = AF_INET; //地址家族
	saServer.sin_port = htons(SERVER_PORT); //注意转化为网络字节序
	saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //使用INADDR_ANY 指示任意地址 
 
													   //绑定
	ret = bind(sListen, (struct sockaddr *)&saServer, sizeof(saServer));
	if (ret == SOCKET_ERROR)
	{
		printf("bind() faild! code:%d\n", WSAGetLastError());
		closesocket(sListen); //关闭套接字
		WSACleanup();
		return 0;
	}
 
	//侦听连接请求
	ret = listen(sListen, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("listen() faild! code:%d\n", WSAGetLastError());
		closesocket(sListen); //关闭套接字
		return 0;
	}
 
	printf("Waiting for client connecting!\n");
	printf("Tips: Ctrl+c to quit!\n");
	length = sizeof(saClient);
	sServer = accept(sListen, (struct sockaddr *)&saClient, &length);
	if (sServer == INVALID_SOCKET)
	{
		printf("accept() faild! code:%d\n", WSAGetLastError());
		closesocket(sListen); //关闭套接字
		WSACleanup();
		return 0;
	}
 
	//阻塞等待接受客户端连接
	while (!data.fin)
	{
		memset(data.receivemessage, '0', sizeof(data.receivemessage));
		ret = recv(sServer, (char *)&data, sizeof(struct Data), 0);  //第二个参数使用强制类型，为一个数据包
		if (ret == SOCKET_ERROR)
		{
			printf("recv() failed!\n");
			return 0;
		}
		if (ret == 0) //客户端已经关闭连接
		{
			printf("Client has closed the connection\n");
			break;
		}
		fwrite(data.receivemessage, data.length, 1, fp);
	}
 
	  closesocket(sListen);
	  closesocket(sServer);
	  WSACleanup();
	return 0;
}

