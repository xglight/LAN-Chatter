#include <cstdio>
#include "winsock2.h"
using namespace std;
char recvBuf[100];
SOCKET sockConn;
DWORD WINAPI get_message(LPVOID lpParamter) {
    while (true) {
        memset(recvBuf, 0, sizeof(recvBuf));
        recv(sockConn, recvBuf, sizeof(recvBuf), 0);
        printf("%s\n", recvBuf);
    }
    closesocket(sockConn);
}
const int port = 8888; //�˿�
int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("����������ʧ�ܣ�");
        return 0;
    }
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
    if (retVal == SOCKET_ERROR) {
        printf("����ʧ��:%d\n", WSAGetLastError());
        return 0;
    }

    if (listen(sockSrv, 10) == SOCKET_ERROR) {
        printf("����ʧ��:%d", WSAGetLastError());
        return 0;
    }

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    while (1) {
        //�ȴ��ͻ���
        sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
        if (sockConn == SOCKET_ERROR) {
            printf("%d", WSAGetLastError());
            break;
        }
        printf("�ͻ���IP:[%s]\n", inet_ntoa(addrClient.sin_addr));
        char sendbuf[] = "��ã������Ƿ���ˡ�";
        int iSend = send(sockConn, sendbuf, sizeof(sendbuf), 0);
        if (iSend == SOCKET_ERROR) {
            printf("");
            break;
        }
        HANDLE hThread = CreateThread(NULL, 0, get_message, NULL, 0, NULL);
        CloseHandle(hThread);
    }

    closesocket(sockSrv);
    WSACleanup();
    system("pause");
    return 0;
}
