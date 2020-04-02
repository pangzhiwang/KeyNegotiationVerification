#include <iostream>
#include "Codec.h"
#include "Message.pb.h"
#include "RequestCodec.h"
#include "RespondMsgCodec.h"
#include "CodecFactory.h"
#include "RequestFactory.h"
#include "RespondFactory.h"

using namespace std;

// 编码
string encodeMsg(Codec* codec)
{
    return codec->encodeMsg();
}

// 解码
void* decodeMsg(Codec* codec)
{
    return codec->decodeMsg();
}

int main()
{
    RequestInfo info;
    info.cmd = 1;
    info.clientID = "Mac";
    info.serverID = "Centos";
    info.sign = "sniper的mac";
    info.data = "这里是sniper_pangzhi的mac";
    // 编码
    //RequestFactory request_enc(&info);
    CodecFactory* factory = (CodecFactory*)new RequestFactory(&info);
    Codec* req =NULL;
    req = factory->CreateCodec();
    string reqMsg = encodeMsg(req);
    cout << "req编码之后的结果:" << reqMsg << endl;
    delete factory;
    delete req;
    req = NULL;
    // 解码
    factory = (CodecFactory*)new RequestFactory(reqMsg);
    req = factory->CreateCodec();
    RequestMsg* reqDecMsg = (RequestMsg*)decodeMsg(req);
    cout << "cmdType: "<< reqDecMsg->cmdtype()
        <<"\tclientID: "<<reqDecMsg->clientid()
        <<"\tserverID: "<<reqDecMsg->serverid()
        <<"\tsign: "<<reqDecMsg->sign()
        <<"\tdata: "<<reqDecMsg->data() <<endl;
    delete factory;
    delete req;
    req = NULL;
    
    cout << endl << "=========================" << endl;
    
    RespondInfo resInfo;
    resInfo.status = 1;
    resInfo.seckeyID = 100;
    resInfo.clientID = "Mac";
    resInfo.serverID = "Centos";
    resInfo.data = "这里是客户端Centos";
    // 编码
    factory = (CodecFactory*)new RespondFactory(&resInfo);
    Codec* res = NULL;
    res = factory->CreateCodec();
    cout << "respond:" << res << endl;
    string resMsg = encodeMsg(res);
    cout<<"respond编码之后的结果:"<< resMsg <<endl;
    delete factory;
    delete res;
    res = NULL;
    // 解码
    factory = (CodecFactory*)new RespondFactory(resMsg);
    res = factory->CreateCodec();
    RespondMsg* resMsgDec = (RespondMsg*)decodeMsg(res);
    cout<<"status: "<<resMsgDec->status()
        <<"\tseckeyID: "<<resMsgDec->seckeyid()
        <<"\tclientID: "<<resMsgDec->clientid()
        <<"\tserverID: "<<resMsgDec->serverid()
        <<"\tdata: "<<resMsgDec->data()<<endl;
    delete factory;
    delete res;
    res = NULL;
    return 0;
}
