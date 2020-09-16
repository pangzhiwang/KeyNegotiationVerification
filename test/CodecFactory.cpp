#include "CodecFactory.h"

CodecFactory::CodecFactory()
{

}

Codec* CodecFactory::CreateCodec()
{
    return NULL;
}

int CodecFactory::DeleteCodec(Codec* codec)
{
    return 0;
}

CodecFactory::~CodecFactory()
{

}
