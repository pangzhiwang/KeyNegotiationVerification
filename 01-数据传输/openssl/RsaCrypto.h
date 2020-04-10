#pragma once
#include <iostream>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

enum SignLevel
{
    L_MD5 = NID_md5,
    L_SHA1 = NID_sha1,
    L_SHA224 = NID_sha224,
    L_SHA256 = NID_sha256,
    L_SHA384 = NID_sha384,
    L_SHA512 = NID_sha512
};

class RsaCrypto
{
public:
    RsaCrypto();
    RsaCrypto(string filename, bool isPrivate = true);
    ~RsaCrypto();

    // 创建密钥对
    void generateRsakey(int bits, string pub = "public.pem", string pri = "private.pem");
    // 公钥加密
    string rsaPubKeyEncrypt(string data);
    // 私钥加密
    string rsaPriKeyDecrypt(string data);
    // rsa签名
    string rsaSign(string data, SignLevel level = L_SHA224);
    // 校验签名
    bool rsaVerify(string data, string signData, SignLevel level = L_SHA224);
   
private:
    // 获取公钥
    bool getPublicKey(string pubfile);
    // 获取私钥
    bool getPrivateKey(string prifile);
private:
    RSA* m_publicKey;   // 公钥
    RSA* m_privateKey;  // 私钥
};
