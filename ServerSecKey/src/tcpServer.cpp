#include "tcpServer.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

tcpServer::tcpServer()
{
}

tcpServer::~tcpServer()
{
}

int tcpServer::setListen(unsigned short port)
{
    int ret = 0;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 创建套接字
    m_lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == m_lfd)
    {
        ret = errno;
        return ret;
    }

    int on = 1;
    ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(-1 == ret)
    {
        ret = errno;
        return ret;
    }

    ret = bind(m_lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(-1 == ret)
    {
        ret = errno;
        return ret;
    }

    ret = listen(m_lfd, 128);
    if(-1 == ret)
    {
        ret = errno;
        return ret;
    }

    return ret;
}

tcpSocket* tcpServer::acceptConn(int wait_seconds)
{
    int ret;
    if(wait_seconds > 0)
    {
        fd_set accept_fdset;
        struct timeval timeout;
        FD_ZERO(&accept_fdset);
        FD_SET(m_lfd, &accept_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(m_lfd+1, &accept_fdset, NULL, NULL, &timeout);
        }while(ret<0 && EINTR == errno);
        if(ret <= 0)
        {
            return NULL;
        }
    }

    struct sockaddr_in addrCli;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int connfd = accept(m_lfd, (struct sockaddr*)&addrCli, &addrlen);
    if(-1 == connfd)
    {
        return NULL;
    }

    return new tcpSocket(connfd);
}

void tcpServer::closefd()
{
    close(m_lfd);
}
