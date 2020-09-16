#include <iostream>
#include "RsaCrypto.h"
#include <string>

using namespace std;

int main()
{
    string data = "Hello,world!";
    // 创建密钥对
    RsaCrypto rsa;
    rsa.generateRsakey(2048);
    
    // 加密数据
    string dataEnc = rsa.rsaPubKeyEncrypt(data);
    cout << "加密后的数据:" << dataEnc << endl;
    
    // 解密数据
    string dataDec = rsa.rsaPriKeyDecrypt(dataEnc);
    cout << "解密之后的数据:" << dataDec << endl;

    // 签名
    string sign = rsa.rsaSign(data);
    // 校验签名
    bool ret = rsa.rsaVerify(data, sign);
    cout << "数据校验结果：" << ret << endl;
    return 0;
}

