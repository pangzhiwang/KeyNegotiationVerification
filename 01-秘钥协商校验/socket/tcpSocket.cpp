#include "tcpSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


tcpSocket::tcpSocket()
{
}

tcpSocket::tcpSocket(int connfd)
{
    m_socket = connfd;
}

tcpSocket::~tcpSocket()
{
}

int tcpSocket::connectToHost(string ip, unsigned short port, int timeout)
{
    int ret = 0;
    // 判断port和timeout是否在规定范围内
    if(port<0 || port>65535 || timeout<0)
    {
        ret = ParamError;
        return ret;
    }
    // 创建套接字
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(ret<0)
    {
        ret = errno;
        printf("func socket() err: %d\n", ret);
        return ret;
    }
    
    // 链接服务器
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); // 清零
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip.data());
    
    ret = connectTimeout((struct sockaddr_in *)(&servaddr), (unsigned int)timeout);
    if(ret < 0)
    {
        if((ret == -1) && (errno == ETIMEDOUT))
        {
            ret = TimeoutError;
            return ret;
        }
        else
        {
            printf("connectTimeout 调用一场，错误号: %d\n",errno);
            return errno;
        }
    }

    return ret;
}

int tcpSocket::sendMsg(string sendData, int timeout)
{
    // 没超时：0 超时：-1
    int ret = writeTimeout(timeout);
    if(0 == ret)
    {
        int writed = 0;
        int dataLen = sendData.size() + 4; // 其中4字节数据头
        unsigned char *netdata = (unsigned char *)malloc(dataLen);
        if(NULL == netdata)
        {
            ret = MallocError;
            printf("func sendMsg() malloc Err:%d\n", ret);
            return ret;
        }
        int netlen = htonl(sendData.size());
        memcpy(netdata, &netlen, 4);
        memcpy(netdata+4, sendData.data(), sendData.size());
        // 发送数据，发送成功返回发送实际字节数，失败返回-1
        writed = writen(netdata, dataLen);
        if(writed < dataLen) // 发送失败
        {
            if(NULL != netdata)
            {
                free(netdata);
                netdata = NULL;
            }
            return writed;
        }
        if(NULL != netdata)
        {
            free(netdata);
            netdata = NULL;
        }
    }
    else
    {
        if((-1 == ret) && (errno == ETIMEDOUT))
        {
            ret = TimeoutError;
            printf("func sendMsg() malloc Err:%d\n", ret);
        }
    }

    return ret;
}

string tcpSocket::recvMsg(int timeout)
{
    int ret = readTimeout(timeout);
    if(ret != 0)
    {
        if(-1 == ret || ETIMEDOUT == errno)
        {
            printf("readTimeout() Err: TimeoutError\n");
            return string();
        }
        else
        {
            printf("readTimeout() Err:%d\n", ret);
            return string();
        }
    }

    int netdatalen = 0;
    ret = readn(&netdatalen, 4); // 读包头 4字节
    if(-1 == ret)
    {
        printf("func readn() Err:%d\n", ret);
        return string();
    }
    else if (ret < 4)
    {
        printf("func readn() err peer close:%d\n", ret);
        return string();
    }

    int n = ntohl(netdatalen);
    char *tmpBuf = (char *)malloc(n+1);
    if(NULL == tmpBuf)
    {
        ret = MallocError;
        printf("malloc() err\n");
        return NULL;
    }

    ret = readn(tmpBuf, n);
    if(-1 == ret)
    {
        printf("func readn() err:%d\n",ret);
        return string();
    }
    else if(ret<n)
    {
        printf("func readn() err perr close:%d\n", ret);
        return string();
    }

    tmpBuf[n] = '\0';
    string data = string(tmpBuf);

    free(tmpBuf);

    return data;
}

void tcpSocket::disConnect()
{
    if(m_socket >= 0)
    {
        close(m_socket);
    }
}


// 设置I/O为非阻塞模式
int tcpSocket::setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if(-1 == flags)
    {
        return flags;
    }

    flags |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flags);
    return ret;
}

// 设置I/O为阻塞模式
int tcpSocket::setBlock(int fd)
{
    int ret = 0;
    int flags = fcntl(fd, F_GETFL);
    if(-1 == flags)
    {
        return flags;
    }

    flags &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    return ret;
}

int tcpSocket::readTimeout(unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0)
    {
        fd_set read_fdset;
        struct timeval timeout;

        FD_ZERO(&read_fdset);
        FD_SET(m_socket, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
            ret = select(m_socket+1, &read_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(0 == ret)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(1 == ret)
        {
            ret = 0;
        }
    }

    return ret;
}


int tcpSocket::writeTimeout(unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0)
    {
        fd_set write_fdset;
        struct timeval timeout;

        FD_ZERO(&write_fdset);
        FD_SET(m_socket, &write_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
            ret = select(m_socket+1, NULL, &write_fdset, NULL, &timeout);
        }while(ret<0 && errno == EINTR);

        if(0 == ret)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(1 == ret)
        {
            ret = 0;
        }
    }

    return ret;
}


int tcpSocket::connectTimeout(struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if(wait_seconds > 0)
    {
        setNonBlock(m_socket);
    }

    ret = connect(m_socket, (struct sockaddr*)addr, addrlen);
    if(ret < 0 && errno == EINPROGRESS)
    {
        fd_set connect_fdset;
        struct timeval timeout;
        FD_ZERO(&connect_fdset);
        FD_SET(m_socket, &connect_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(m_socket+1, NULL, &connect_fdset, NULL, &timeout);
        }while(ret<0 && errno == EINTR);

        if(0 == ret)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(ret < 0)
        {
            return -1;
        }
        else if(1 == ret)
        {
            int err;
            socklen_t sockLen = sizeof(err);
            int sockoptret = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen);
            if(-1 == sockoptret)
            {
                return -1;
            }
            if(0 == err)
            {
                ret = 0;
            }
            else
            {
                errno = err;
                ret = -1;
            }
        }
    }
    if(wait_seconds>0)
    {
        setBlock(m_socket);
    }

    return ret;
}

int tcpSocket::readn(void* buf, int count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char *)buf;

    while(nleft > 0)
    {
        if((nread = read(m_socket, bufp, nleft)) < 0)
        {
            if(EINTR == errno)
            {
                continue;
            }
            return -1;
        }
        else if(0 == nread)
        {
            return count - nleft;
        }

        bufp += nread;
        nleft -= nread;
    }

    return count;
}

int tcpSocket::writen(const void* buf, int count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char*)buf;

    while(nleft > 0)
    {
        if((nwritten = write(m_socket, bufp, nleft)) < 0)
        {
            if(EINTR == errno)
            {
                continue;
            }
            return -1;
        }
        else if(0 == nwritten)
        {
            continue;
        }

        bufp += nwritten;
        nleft -= nwritten;
    }

    return count;
}
