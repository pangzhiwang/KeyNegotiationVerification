#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
//extern "C"
//{
//#include <openssl/applink.c>
//};

RsaCrypto::RsaCrypto()
{
    m_publicKey = RSA_new();
    m_privateKey = RSA_new();
}

RsaCrypto::RsaCrypto(string filename, bool isPrivate)
{
    m_publicKey = RSA_new();
    m_privateKey = RSA_new();

    if(isPrivate)
    {
        getPrivateKey(filename);
    }
    else
    {
        getPublicKey(filename);
    }
}

RsaCrypto::~RsaCrypto()
{
    RSA_free(m_publicKey);
    RSA_free(m_privateKey);
}

void RsaCrypto::generateRsakey(int bits, string pub, string pri)
{
    // 生成密钥对
    RSA* r = RSA_new();     // 创建RSA对象
    BIGNUM* e = BN_new();   // 创建bignum对象
    BN_set_word(e, 456787); // 初始化bignum对象
    RSA_generate_key_ex(r, bits, e, NULL);  // 生成密钥对

    // 保存公钥到本地
    BIO* pubBIO = BIO_new_file(pub.data(), "w"); // 创建bio对象
    PEM_write_bio_RSAPublicKey(pubBIO, r);      // 将公钥写入文件中
    BIO_flush(pubBIO);  // 将缓存中的数据写入到文件中
    BIO_free(pubBIO);   // 释放BIO对象
    // 保存私钥到本地
    BIO* priBIO = BIO_new_file(pri.data(), "w"); // 创建bio对象
    PEM_write_bio_RSAPrivateKey(priBIO, r, NULL, NULL, 0, NULL, NULL); // 将私钥保存到文件中
    BIO_flush(priBIO);  // 将缓存中的数据写入磁盘
    BIO_free(priBIO);   // 释放bio对象

    // 获取公私钥
    m_privateKey = RSAPrivateKey_dup(r);    // 获取私钥
    m_publicKey = RSAPublicKey_dup(r);      // 获取私钥

    // 释放资源
    BN_free(e);
    RSA_free(r);
}

// 从文件中读取公钥
bool RsaCrypto::getPublicKey(string pubfile)
{
    // 创建BIO对象
    BIO* pubBio = BIO_new_file(pubfile.data(), "r");
    if(PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL) == NULL)
    {
        ERR_print_errors_fp(stdout);
        return false;
    }
    // 释放资源
    BIO_free(pubBio);

    return true;
}

// 从文件中读取私钥
bool RsaCrypto::getPrivateKey(string prifile)
{
    // 创建bio文件
    BIO* priBio = BIO_new_file(prifile.data(), "r");

    if(PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL) == NULL)
    {
        ERR_print_errors_fp(stdout);
        return false;
    }

    // 释放资源
    BIO_free(priBio);

    return true;
}

string RsaCrypto::rsaPubKeyEncrypt(string data)
{
    // 计算公钥长度
    int keyLen = RSA_size(m_publicKey);

    // 申请内存空间
    char *encode = new char[keyLen+1];
    // 公钥加密
    int ret = RSA_public_encrypt(data.size(),
                                (const unsigned char*)data.data(),
                                (unsigned char*)encode, m_publicKey,
                                RSA_PKCS1_OAEP_PADDING);
    string retstr = string();
    if(ret >=0)
    {
        retstr = string(encode, ret);
    }

    // 释放资源
    delete[] encode;
    return retstr;
}

string RsaCrypto::rsaPriKeyDecrypt(string data)
{
    // 计算私钥长度
    int keyLen = RSA_size(m_privateKey);
    cout << "私钥长度:" << keyLen << endl;
    // 私钥解密
    char *decode = new char[keyLen+1];
    int ret = RSA_private_decrypt(data.size(), 
                                (const unsigned char*)data.data(),
                                (unsigned char*)decode, m_privateKey,
                                RSA_PKCS1_OAEP_PADDING);
    cout << "加密后长度：" << ret << endl;
    string retstr = string();
    if(ret >= 0)
    {
        retstr = string(decode, ret);
    }

    // 释放资源
    delete[] decode;
    return retstr;
}

// 签名
string RsaCrypto::rsaSign(string data, SignLevel level)
{
    unsigned int len;
    char *signBuf = new char[RSA_size(m_privateKey)+1];
    RSA_sign(level, (const unsigned char*)data.data(), data.size(),
             (unsigned char*)signBuf, &len, m_privateKey);
    string retstr = string(signBuf, len);
    delete []signBuf;
    return retstr;
}

bool RsaCrypto::rsaVerify(string data, string signData, SignLevel level)
{
    // 验证签名
    int ret = RSA_verify(level, (const unsigned char*)data.data(),
                         data.size(),
                         (const unsigned char*)signData.data(), 
                         signData.size(), m_privateKey);
    if(ret != 1)
    {
        return false;
    }
    return true;
}
