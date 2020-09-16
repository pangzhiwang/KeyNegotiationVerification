#pragma once
#include <iostream>
#include <string>

using namespace std;

class Codec
{
public:
    Codec();
    virtual string encodeMsg();
    virtual void *decodeMsg();

    virtual ~Codec();
};

