#pragma once
#include <iostream>
#include <string>
#include "Codec.h"
#include "Message.pb.h"

using namespace std;

struct RequestInfo
{
    int cmd;
    string clientID;
    string serverID;
    string sign;
    string data;
};

class RequestCodec: public Codec
{
public:
    // 构造空对象
    RequestCodec();
    // 构造对象，用于解码
    RequestCodec(string encstr);
    // 构造对象，用于编码
    RequestCodec(RequestInfo *info);
    // 初始化对象，用于解码
    void initMessage(string encstr);
    // 初始化对象，用于编码
    void initMessage(RequestInfo *info);
    // 编码（重写父类函数）
    string encodeMsg();
    // 解码（重写父类函数）
    void *decodeMsg();
    // 析构
    ~RequestCodec();

private:
    string m_encStr;
    RequestMsg m_msg;
};
