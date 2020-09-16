#pragma once
#include <iostream>
#include "Message.pb.h"
#include <string>
#include "Codec.h"

using namespace std;

struct RespondInfo
{
    bool status;
    int seckeyID;
    string clientID;
    string serverID;
    string data;
};

class RespondMsgCodec:public Codec
{
public:
    // 构造空对象
    RespondMsgCodec();
    // 构造对象，用于解码
    RespondMsgCodec(string encstr);
    // 构造对象，用于编码
    RespondMsgCodec(RespondInfo *info);
    // 构造对象，用于解码
    void initMessage(string encstr);
    // 构造对象，用于编码
    void initMessage(RespondInfo *info);
    // 编码
    string encodeMsg();
    // 解码
    void* decodeMsg();
    // 析构
    ~RespondMsgCodec();

private:
    // 用于存储解码内容
    string m_encStr;
    // 用于存储需要编码的内容
    RespondMsg m_msg;
};
