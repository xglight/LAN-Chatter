#include <cstring>
#include <handleapi.h>
#include <iostream>
#include <minwindef.h>
#include <stdlib.h>
#include <stdio.h>
#include "winsock2.h"
using namespace std;
const int port = 8888;
char recvbuf[1505];
SOCKET sockClient;
DWORD WINAPI get_message(LPVOID lpParam) {
    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));
        int result = recv(sockClient, recvbuf, sizeof(recvbuf), 0);
        if (result == 0) {
            printf("������ѶϿ�\n");
            closesocket(sockClient);
            // WSACleanup();
            break;
        } else if (result > 0)
            printf("%s\n", recvbuf);
        else {
            printf("����˳��ִ����ѶϿ�\n");
            closesocket(sockClient);
            break;
        }
    }
    return 0;
}
char username[1024], ip[1024];
char message[1505], ch;
WSADATA wsaData;
SOCKADDR_IN addrSrv;
int main() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("��ʼ��ʧ��");
        return 0;
    }
    printf("�������û�����");
    scanf("%s", username);
    printf("�����������IP��ַ��");
    scanf("%s", ip);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);

    sockClient = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_ERROR == sockClient) {
        printf("Socket����ʧ��:%d", WSAGetLastError());
        return 0;
    }
    if (connect(sockClient, (struct sockaddr *)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
        printf("����ʧ��:%d", WSAGetLastError());
        return 0;
    } else {
        recv(sockClient, recvbuf, sizeof(recvbuf), 0);
        printf("%s\n", recvbuf);
    }
    send(sockClient, username, sizeof(username), 0);
    HANDLE hThread = CreateThread(NULL, 0, get_message, NULL, 0, NULL);
    CloseHandle(hThread);
    cin.getline(message, sizeof(message));
    while (1) {
        bool f = 0;
        while ((ch = getchar()) != EOF) {
            f = 1;
            if (ch == '\n') break;
            sprintf(message, "%s%c", message, ch);
        }
        if (!f) break;
        if (strcmp(message, "") == 0 || strcmp(message, "\n") == 0) continue;
        send(sockClient, message, sizeof(message), 0);
        memset(message, 0, sizeof(message));
    }
    closesocket(sockClient);
    WSACleanup();
    system("pause");
    return 0;
}