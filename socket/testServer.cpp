#include <iostream>
#include "tcpServer.h"
#include <string.h>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *callback(void *arg)
{
    tcpSocket* sockTcp = (tcpSocket*)arg;

    while(1)
    {
        string msg = sockTcp->recvMsg();
        if(msg.empty())
        {
            break;
        }
        cout << "recvMsg:" << msg << endl;
        string sendMsg = "Hello, client...";
        sockTcp->sendMsg(sendMsg);
    }
}

int main()
{
    tcpServer *server = new tcpServer;
    server->setListen(8080);
    while(1)
    {
        cout << "开始accept..." << endl;
        tcpSocket* socktcp = server->acceptConn();
        cout << "接受一个客户端的链接请求..." << endl;
        if(socktcp)
        {
            pthread_t tid;
            pthread_create(&tid, NULL, callback, socktcp);
            pthread_detach(tid);
        }
    }
    return 0;
}
