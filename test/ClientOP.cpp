#include "ClientOP.h"
#include <json/json.h>
#include <fstream>
#include <sstream>
#include "RequestFactory.h"
#include "RequestCodec.h"
#include "RespondFactory.h"
#include "RespondMsgCodec.h"
#include "Message.pb.h"
#include "RsaCrypto.h"
#include "Hash.h"
#include "tcpSocket.h"

using namespace std;
using namespace Json;

ClientOP::ClientOP(string jsonFile)
{
	cout << "9999999999" << endl;
    // 读取json文件
    ifstream ifs(jsonFile);
    Reader r;
    Value root;
    r.parse(ifs, root);

    m_info.serverID = root["ServerID"].asString();
    m_info.clientID = root["ClientID"].asString();
    m_info.ip = root["ServerIP"].asString();
    m_info.port = root["Port"].asInt();

    cout << "服务器端地址:" << m_info.ip << "\t端口:" << m_info.port << endl;
}

ClientOP::~ClientOP()
{
}

bool ClientOP::seckeyAgree()
{
    // 创建rsa对象
    RsaCrypto rsa;
    // 创建密钥对
    rsa.generateRsakey(1024);
    // 读取公钥
    ifstream ifs("public.pem");
    stringstream str;
    str << ifs.rdbuf();

    // 初始化序列化数据
    RequestInfo reqInfo;
    reqInfo.cmd = 1; // 密钥协商
    reqInfo.clientID = m_info.clientID;
    reqInfo.serverID = m_info.serverID;

    Hash ha(H_SHA1);
    ha.addData(str.str());
    //reqInfo.sign = rsa.rsaSign(ha.result()); // 公钥签名
    reqInfo.sign = ha.result();
    reqInfo.data = str.str(); // 非对称加密的公钥
    // 编码
    //RequestFactory request_enc(&info);
    CodecFactory* factory = (CodecFactory*)new RequestFactory(&reqInfo);
    Codec* req =NULL;
    req = factory->CreateCodec();
    string reqMsg = req->encodeMsg();
    cout << "req编码之后的结果:" << reqMsg << endl;
    delete factory;
    delete req;
    req = NULL;
    // 解码
    factory = (CodecFactory*)new RequestFactory(reqMsg);
    req = factory->CreateCodec();
    RequestMsg* reqDecMsg = (RequestMsg*)req->decodeMsg();
    cout << "cmdType: "<< reqDecMsg->cmdtype()
        <<"\tclientID: "<<reqDecMsg->clientid()
        <<"\tserverID: "<<reqDecMsg->serverid()
        <<"\tsign: "<<reqDecMsg->sign()
        <<"\tdata: "<<reqDecMsg->data() <<endl;
    delete factory;
    delete req;
    req = NULL;

    return true;
}

void ClientOP::seckeyCheck()
{

}

void ClientOP::seckeyCancel()
{

}
