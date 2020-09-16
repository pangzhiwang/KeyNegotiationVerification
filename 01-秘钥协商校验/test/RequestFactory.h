#pragma once
#include "RequestCodec.h"
#include "CodecFactory.h"

using namespace std;

class RequestFactory:public CodecFactory
{
public:
    RequestFactory(string encstr);
    RequestFactory(RequestInfo *info);
    Codec* CreateCodec();
    int DeleteCodec(Codec* codec);
    ~RequestFactory();

private:
    bool flag;
    string m_encStr;
    RequestInfo *m_info;
};

