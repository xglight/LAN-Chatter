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
        printf("��ʼ��Winsockʧ��");
        return 0;
    }

    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);                        //�˿ں�
    addrSrv.sin_addr.S_un.S_addr = inet_addr("10.0.9.31"); //IP��ַ

    //�����׽���
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR == sockClient) {
        printf("Socket() error:%d", WSAGetLastError());
        return 0;
    }

    //�������������������
    if (connect(sockClient, (struct sockaddr *)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
        printf("����ʧ��:%d", WSAGetLastError());
        return 0;
    } else {
        //��������
        recv(sockClient, buff, sizeof(buff), 0);
        printf("%s\n", buff);
    }

    //��������
    char buffs[] = "";
    //�������룬Ȼ����
    while (true) {
        cin >> buffs;
        send(sockClient, buffs, sizeof(buffs), 0);
    }

    //�ر��׽���
    closesocket(sockClient);
    WSACleanup();    //�ͷų�ʼ��Ws2_32.dll���������Դ��
    system("pause"); //����Ļ����
    return 0;
}