#pragma once
#include <iostream>
#include "CodecFactory.h"
#include "RespondMsgCodec.h"

using namespace std;

class RespondFactory:public CodecFactory
{
public:
    RespondFactory(string encstr);
    RespondFactory(RespondInfo *info);
    Codec* CreateCodec();
    int DeleteCodec(Codec *codec);
    ~RespondFactory();

private:
    bool flag;
    string m_encStr;
    RespondInfo *m_info;
};
