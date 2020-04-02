#include "RespondMsgCodec.h"

RespondMsgCodec::RespondMsgCodec()
{

}

RespondMsgCodec::RespondMsgCodec(string encstr)
{
    initMessage(encstr);
}

RespondMsgCodec::RespondMsgCodec(RespondInfo *info)
{
    initMessage(info);
}

void RespondMsgCodec::initMessage(string encstr)
{
    m_encStr = encstr;
}

void RespondMsgCodec::initMessage(RespondInfo *info)
{
    m_msg.set_status(info->status);
    m_msg.set_seckeyid(info->seckeyID);
    m_msg.set_clientid(info->clientID);
    m_msg.set_serverid(info->serverID);
    m_msg.set_data(info->data);
}

string RespondMsgCodec::encodeMsg()
{
    string output;
    m_msg.SerializeToString(&output);
    return output;
}

void* RespondMsgCodec::decodeMsg()
{
    m_msg.ParseFromString(m_encStr);
    return &m_msg;
}

RespondMsgCodec::~RespondMsgCodec()
{

}
