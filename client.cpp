#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "winsock2.h"
using namespace std;
const int port = 8888;
int main() {
    WSADATA wsaData;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("初始化Winsock失败");
        return 0;
    }

    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);                        //端口号
    addrSrv.sin_addr.S_un.S_addr = inet_addr("10.0.9.31"); //IP地址

    //创建套接字
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR == sockClient) {
        printf("Socket() error:%d", WSAGetLastError());
        return 0;
    }

    //向服务器发出连接请求
    if (connect(sockClient, (struct sockaddr *)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
        printf("连接失败:%d", WSAGetLastError());
        return 0;
    } else {
        //接收数据
        recv(sockClient, buff, sizeof(buff), 0);
        printf("%s\n", buff);
    }

    //发送数据
    char buffs[] = "";
    //不断输入，然后发送
    while (true) {
        cin >> buffs;
        send(sockClient, buffs, sizeof(buffs), 0);
    }

    //关闭套接字
    closesocket(sockClient);
    WSACleanup();    //释放初始化Ws2_32.dll所分配的资源。
    system("pause"); //让屏幕暂留
    return 0;
}