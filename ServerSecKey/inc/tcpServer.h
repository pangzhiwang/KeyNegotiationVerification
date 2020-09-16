#pragma once
#include "tcpSocket.h"

class tcpServer
{
public:
    tcpServer();
    ~tcpServer();

    // 监听
    int setListen(unsigned short port);
    // 等待客户端链接, 默认链接超时时间为10000s
    tcpSocket* acceptConn(int timeout = 10000);
    void closefd();
private:
    int m_lfd;
};

