#pragma once
#include "Codec.h"
using namespace std;

class CodecFactory
{
public:
    CodecFactory();
    virtual ~CodecFactory();
    virtual Codec* CreateCodec();
    virtual int DeleteCodec(Codec* codec);
};
