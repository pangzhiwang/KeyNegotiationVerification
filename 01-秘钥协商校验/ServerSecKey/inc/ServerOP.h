#pragma once
#include <map>
#include <string>
#include <json/json.h>
#include "tcpSocket.h"
#include "Message.pb.h"
#include "tcpServer.h"

using namespace std;
using namespace Json;

class ServerOP
{
public:
    ServerOP(string json);
    ~ServerOP();
    // 启动服务器
    void startServer();
    // 线程工作函数
    static void* working(void *arg);
    friend void* workHard(void *arg);

    string seckeyAgree(RequestMsg* msg);

private:
    string getRandStr(int num); // 获取随机字符串

private:
    string m_serverID;
    unsigned short m_port;
    map<pthread_t, tcpSocket*> m_list;
    tcpServer* m_server = NULL; 
};

void *workHard(void *arg);
