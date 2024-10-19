#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <minwindef.h>
#include <psdk_inc/_socket_types.h>
#include <set>
#include <string>
#include "winsock2.h"
using namespace std;
int cnt = 1;
set<int> id_free, id_have;
map<string, int> client;
SOCKET sockcli[1005];
DWORD WINAPI client_server(LPVOID lpParamter) {
    char recvBuf[1505], sendBuf[1505];
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
char recvBuf[1505], sendBuf[1505];
int port = 8888; // �˿�
char ip[1024][1024];
WSADATA wsaData;
SOCKADDR_IN addrSrv;
SOCKADDR_IN addrClient[1005];
HANDLE hThread[1005], cThread[1005];
int ic = -1;
DWORD WINAPI connect_client(LPVOID lpParamter) {
    int id = 0;
    for (int i = strlen((char *)lpParamter) - 1; i >= 0; i--)
        id = id * 10 + ((char *)lpParamter)[i] - '0';
    sprintf(sendBuf, "����ˣ���ã���ӭ���ӵ������ IP:[%s]��", ip[ic]);
    int iSend = send(sockcli[id], sendBuf, sizeof(sendBuf), 0);
    if (iSend == SOCKET_ERROR) {
        printf("����ˣ��пͻ������ӣ���������ʧ��:%d]\n", WSAGetLastError());
        return 0;
    }
    bool f = 0;
    do {
        memset(recvBuf, 0, sizeof(recvBuf));
        int result = recv(sockcli[id], recvBuf, sizeof(recvBuf), 0);
        if (result == 0) {
            sprintf(sendBuf, "����ˣ��ͻ��� IP:[%s] �ѶϿ�", inet_ntoa(addrClient[id].sin_addr));
            printf("%s\n", sendBuf);
            closesocket(sockcli[id]);
            for (auto i = id_have.begin(); i != id_have.end(); i++) {
                if (*i == id) continue;
                send(sockcli[*i], sendBuf, strlen(sendBuf), 0);
            }
            client[(char *)recvBuf] = 0;
            id_free.insert(id), id_have.erase(id);
            break;
        } else if (result < 0) {
            sprintf(sendBuf,
                    "����ˣ��ͻ��� IP:[%s] ���ִ����ѶϿ�:%d",
                    inet_ntoa(addrClient[id].sin_addr),
                    WSAGetLastError());
            printf("%s\n", sendBuf);
            closesocket(sockcli[id]);
            client[(char *)recvBuf] = 0;
            for (auto i = id_have.begin(); i != id_have.end(); i++) {
                if (*i == id) continue;
                send(sockcli[*i], sendBuf, strlen(sendBuf), 0);
            }
            id_free.insert(id), id_have.erase(id);
            break;
        }
        if (client[(char *)recvBuf] != 0) {
            sprintf(sendBuf, "����ˣ��û���%s�ѱ�ռ�ã������������û�����", recvBuf);
            send(sockcli[id], sendBuf, sizeof(sendBuf), 0);
        } else {
            client[(char *)recvBuf] = id;
            sprintf(sendBuf, "����ˣ���ӭ�ͻ���%s IP:[%s]��", recvBuf, inet_ntoa(addrClient[id].sin_addr));
            send(sockcli[id], sendBuf, sizeof(sendBuf), 0);
            for (auto i = id_have.begin(); i != id_have.end(); i++) {
                if (*i == id) continue;
                send(sockcli[*i], sendBuf, strlen(sendBuf), 0);
            }
            break;
        }
    } while (1);
    printf("���ӵ��ͻ���:%s IP:[%s]\n", recvBuf, inet_ntoa(addrClient[id].sin_addr));
    hThread[id] = CreateThread(NULL, 0, client_server, (LPVOID)recvBuf, 0, NULL);
    CloseHandle(hThread[id]);
    id_free.erase(id), id_have.insert(id);
    return 0;
}
int main() {
    printf("����������˿ڣ�");
    scanf("%d", &port);
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("����������ʧ�ܣ�\n");
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
        printf("socket����ʧ��:%d\n", WSAGetLastError());
        return 0;
    }
    char hostname[128];
    gethostname(hostname, 128);
    hostent *temp;
    temp = gethostbyname(hostname);
    for (int i = 0; temp->h_addr_list[i] != NULL && i < 10; i++) {
        strcpy(ip[i], inet_ntoa(*(struct in_addr *)temp->h_addr_list[i]));
        ic++;
    }
    printf("����������ɹ���IP:[%s] �����˿ڣ�%d\n", ip[ic], port);
    int len = sizeof(SOCKADDR);
    for (int i = 1; i <= 1000; i++)
        id_free.insert(i);
    client.clear();
    while (1) {
        // �ȴ��ͻ���
        cnt = *id_free.begin();
        sockcli[cnt] = accept(sockSrv, (SOCKADDR *)&addrClient[cnt], &len);
        if (sockcli[cnt] == SOCKET_ERROR) {
            printf("%d", WSAGetLastError());
            break;
        }
        char chcnt[1005];
        sprintf(chcnt, "%d", cnt);
        cThread[cnt] = CreateThread(NULL, 0, connect_client, (LPVOID)chcnt, 0, NULL);
        CloseHandle(cThread[cnt]);
        id_free.erase(cnt), id_have.insert(cnt);
    }

    closesocket(sockSrv);
    WSACleanup();
    system("pause");
    return 0;
}
