#include "RespondFactory.h"

RespondFactory::RespondFactory(string encstr)
{
    flag = false;
    m_encStr = encstr;
}

RespondFactory::RespondFactory(RespondInfo *info)
{
    flag = true;
    m_info = info;
}

Codec* RespondFactory::CreateCodec()
{
    Codec* codec = NULL;
    if(flag)
    {
        codec = (Codec*)new RespondMsgCodec(m_info);
    }
    else
    {
        codec = (Codec*)new RespondMsgCodec(m_encStr);
    }
    return codec;
}

int RespondFactory::DeleteCodec(Codec* codec)
{
    if(NULL != codec)
    {
        delete (RespondMsgCodec*)codec;
        codec = NULL;
    }
}

RespondFactory::~RespondFactory()
{

}
