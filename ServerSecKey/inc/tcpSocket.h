#pragma once
#include <iostream>

using namespace std;

static const int TIMEOUT = 1000;

class tcpSocket
{
public:
    enum ErrorType {ParamError = 3001, TimeoutError, PeerCloseError, MallocError};
    tcpSocket();
    // 初始化套接字
    tcpSocket(int connfd);
    ~tcpSocket();

    // 链接服务器
    int connectToHost(string ip, unsigned short port, int timeout = TIMEOUT);
    // 发送数据
    int sendMsg(string sendData, int timeout = TIMEOUT);
    // 接收数据
    string recvMsg(int timeout = TIMEOUT);
    // 断开链接
    void disConnect();

private:
    // 设置I/O为非阻塞模式
    int setNonBlock(int fd);
    // 设置I/O为阻塞模式
    int setBlock(int fd);
    // 读超时检测
    int readTimeout(unsigned int wait_seconds);
    // 写超时检测
    int writeTimeout(unsigned int wait_seconds);
    // 带链接超时的connect函数
    int connectTimeout(struct sockaddr_in *addr, unsigned int wait_seconds);
    // 读取n字节数据
    int readn(void *buf, int count);
    // 写n字节数据
    int writen(const void *buf, int count);

private:
    int m_socket;    // 用于通信的套接字
};
