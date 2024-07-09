#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <minwindef.h>
#include <psdk_inc/_socket_types.h>
#include <set>
#include "winsock2.h"
using namespace std;
int cnt = 1;
set<int> id_free, id_have;
map<char *, int> client;
SOCKET sockcli[1005];
char recvBuf[1505], sendBuf[1505];
DWORD WINAPI client_server(LPVOID lpParamter) {
    char username[1005];
    sprintf(username, "%s", (char *)lpParamter);
    int id = client[(char *)lpParamter];
    while (1) {
        memset(recvBuf, 0, sizeof(recvBuf));
        int result = recv(sockcli[id], recvBuf, sizeof(recvBuf), 0);
        if (result == 0) {
            printf("����ˣ��ͻ���%s�ѶϿ�\n", username);
            closesocket(sockcli[id]);
            client[(char *)lpParamter] = 0;
            id_free.insert(id), id_have.erase(id);
            // WSACleanup();
            break;
        } else if (result > 0) {
            printf("%s:%s\n", username, recvBuf);
            sprintf(sendBuf, "%s:%s", username, recvBuf);
            for (auto i = id_have.begin(); i != id_have.end(); i++) {
                if (*i == id) continue;
                send(sockcli[*i], sendBuf, strlen(sendBuf), 0);
            }
        } else {
            printf("����ˣ��ͻ���%s���ִ����ѶϿ�\n", username);
            closesocket(sockcli[id]);
            client[(char *)lpParamter] = 0;
            id_free.insert(id), id_have.erase(id);
            break;
        }
    }
    return 0;
}
const int port = 8888; //�˿�
char ip[1024];
WSADATA wsaData;
SOCKADDR_IN addrSrv;
SOCKADDR_IN addrClient[1005];
HANDLE hThread[1005];
int main() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("����������ʧ�ܣ�");
        return 0;
    }
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
    if (retVal == SOCKET_ERROR) {
        printf("socket����ʧ��:%d\n", WSAGetLastError());
        return 0;
    }

    if (listen(sockSrv, 10) == SOCKET_ERROR) {
        printf("socket����ʧ��:%d", WSAGetLastError());
        return 0;
    }
    strcpy(ip, inet_ntoa(addrSrv.sin_addr));
    printf("����������ɹ���IP:[%s] �����˿ڣ�%d\n", ip, port);
    int len = sizeof(SOCKADDR);
    for (int i = 1; i <= 1000; i++)
        id_free.insert(i);
    while (1) {
        //�ȴ��ͻ���
        cnt = *id_free.begin();
        sockcli[cnt] = accept(sockSrv, (SOCKADDR *)&addrClient[cnt], &len);
        if (sockcli[cnt] == SOCKET_ERROR) {
            printf("%d", WSAGetLastError());
            break;
        }
        char sendbuf[] = "����ˣ���ã������Ƿ���ˡ�";
        int iSend = send(sockcli[cnt], sendbuf, sizeof(sendbuf), 0);
        if (iSend == SOCKET_ERROR) {
            printf("����ˣ��пͻ������ӣ���������ʧ��:%d", WSAGetLastError());
            break;
        }
        recv(sockcli[cnt], recvBuf, sizeof(recvBuf), 0);
        client[(char *)recvBuf] = cnt;
        printf("���ӵ��ͻ���:%s IP:[%s]\n", recvBuf, inet_ntoa(addrClient[cnt].sin_addr));
        hThread[cnt] = CreateThread(NULL, 0, client_server, (LPVOID)recvBuf, 0, NULL);
        CloseHandle(hThread[cnt]);
        id_free.erase(cnt), id_have.insert(cnt);
    }

    closesocket(sockSrv);
    WSACleanup();
    system("pause");
    return 0;
}
