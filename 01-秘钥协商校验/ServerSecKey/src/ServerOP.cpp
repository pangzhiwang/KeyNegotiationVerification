#include <iostream>
#include <unistd.h>
#include <fstream>
#include "ServerOP.h"
#include "RespondMsgCodec.h"
#include "RespondFactory.h"
#include "RequestCodec.h"
#include "RequestFactory.h"
#include "RsaCrypto.h"
#include "Hash.h"
#include "tcpServer.h"
#include "tcpSocket.h"

using namespace std;
using namespace Json;

ServerOP::ServerOP(string json)
{
    // 读取json
    ifstream ifs(json);
    Reader r;
    Value root;
    r.parse(ifs, root); // 将json文件中的内容读取到root中

    m_serverID = root["ServerID"].asString();
    m_port = root["Port"].asInt();
}

void ServerOP::startServer()
{
    m_server = new tcpServer;
    m_server->setListen(m_port);
    while(1)
    {
        cout << "等待客户端连接..." << endl;
        tcpSocket* tcp = m_server->acceptConn();
        if(NULL == tcp)
        {
            cout << "tcp is NULL..." << endl;
            continue;
        }
        cout << "客户端链接成功..." << endl;
        // 创建线程
        pthread_t tid;
        pthread_create(&tid, NULL, working, this);
        m_list.insert(make_pair(tid,tcp));
    }
}

void* ServerOP::working(void *arg)
{
    sleep(1);
    ServerOP* op = (ServerOP*)arg;

    // 接收客户端数据
    tcpSocket* tcp = op->m_list[pthread_self()];
    string msg = tcp->recvMsg();

    // 解码
    CodecFactory* factory = (CodecFactory*)new RequestFactory(msg);
    Codec* c = NULL;
    c = factory->CreateCodec();
    RequestMsg* req = (RequestMsg*)c->decodeMsg();
    /*cout << "cmdType: "<< req->cmdtype()
        <<"\tclientID: "<<req->clientid()
        <<"\tserverID: "<<req->serverid()
        <<"\tsign: "<<req->sign()
        <<"\tdata: "<<req->data() <<endl;*/


    string data;
    switch(req->cmdtype())
    {
    case 1:
        // 密钥协商
        cout << "开始密钥协商..." << endl;
        data = op->seckeyAgree(req);
        break;
    case 2:
        // 密钥验证
        break;
    case 3:
        // 密钥注销
        break;
    default:
        break;
    }
    delete factory;
    delete c;
    c = NULL;
    cout << "发送数据给客户端..." << endl;
    tcp->sendMsg(data);
    tcp->disConnect();
    op->m_list.erase(pthread_self());
    delete tcp;
    return NULL;
}

string ServerOP::seckeyAgree(RequestMsg* req)
{
    if(NULL == req)
    {
        cout << "传进来的参数是NULL" << endl;
    }
    RespondInfo info;
    ofstream ofs("public.pem");
    ofs << req->data();
    ofs.close();

    RsaCrypto rsa("public.pem", false);
    Hash h(H_SHA1); // 创建hash对象
    h.addData(req->data()); // 添加数据
    string sign_test =req->sign(); 
    string data_1 = h.result();
    string sign_data = req->sign();
    bool s = rsa.rsaVerify(data_1, sign_data); // 校验签名
    cout << "校验签名结果：" << s << endl;
    if(!s)
    {
        info.status = false;
        cout << "签名校验失败..." << endl;
    }
    else
    {
        // 生成随机字符串
        string key = getRandStr(16);
        // 公钥加密
        string secStr = rsa.rsaPubKeyEncrypt(key);

        info.clientID = req->clientid();
        info.serverID = m_serverID;
        info.seckeyID = 12;
        info.status = true;
        info.data = secStr;
        cout << "签名校验成功，发送数据给客户端" << endl;
    }

    CodecFactory* factory = (CodecFactory*)new RespondFactory(&info);
    Codec* c = NULL;
    c = factory->CreateCodec();
    string data = c->encodeMsg(); // 获取发送数据

    // 释放资源
    delete factory;
    delete c;
    c = NULL;

    return data;
}

ServerOP::~ServerOP()
{
    if(m_server)
    {
        delete m_server;
    }

}

void* workHard(void *arg)
{
//    sleep(1);
    ServerOP* op = (ServerOP*)arg;

    // 接收客户端数据
    tcpSocket* tcp = op->m_list[pthread_self()];
    string msg = tcp->recvMsg();

    // 解码
    CodecFactory* factory = (CodecFactory*)new RequestFactory(msg);
    Codec* c = NULL;
    c = factory->CreateCodec();
    RequestMsg* req = (RequestMsg*)c->decodeMsg();
    cout << "cmdType: "<< req->cmdtype()
        <<"\tclientID: "<<req->clientid()
        <<"\tserverID: "<<req->serverid()
        <<"\tsign: "<<req->sign()
        <<"\tdata: "<<req->data() <<endl;


    string data;
    switch(req->cmdtype())
    {
    case 1:
        // 密钥协商
        cout << "开始密钥协商..." << endl;
        data = op->seckeyAgree(req);
        break;
    case 2:
        // 密钥验证
        break;
    case 3:
        // 密钥注销
        break;
    default:
        break;
    }
    cout << "发送数据给客户端..." << endl;
    tcp->sendMsg(data);
    delete factory;
    delete c;
    c = NULL;

    return NULL;
}

string ServerOP::getRandStr(int num)
{
    srand(time(NULL));
    string retStr = string();
    char* buf = "~`@#$%^&*()_+=-{}[];':";
    for(int i=0; i<num; ++i)
    {
        int flag = rand() % 4;
        switch(flag)
        {
        case 0:
            retStr.append(1, rand() % 10 + '0');
            break;
        case 1:
            retStr.append(1, rand() % 26 + 'a');
            break;
        case 2:
            retStr.append(1, rand() % 26 + 'A');
            break;
        case 3:
            retStr.append(1, buf[rand() % strlen(buf)]);
            break;
        }
    }

    cout << "获得随机字符串:" << retStr << endl;
    return retStr;
}
