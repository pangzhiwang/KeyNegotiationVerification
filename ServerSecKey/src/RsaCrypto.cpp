#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <iostream>
#include <openssl/buffer.h>
#include <string.h>

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

// 将公钥或者私钥字符串数据解析到RSA对象中
void RsaCrypto::parseKeyString(string filename, bool isPublic)
{
    BIO* bio = BIO_new_mem_buf(filename.data(),filename.size());
    // 公钥
    if(isPublic)
    {
        PEM_read_bio_RSAPublicKey(bio, &m_publicKey, NULL, NULL);
    }
    else
    {
        PEM_read_bio_RSAPrivateKey(bio, &m_publicKey, NULL, NULL);
    }
    BIO_free(bio);
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
    cout << "加密数据长度:" << data.size() << endl;
    // 计算公钥长度
    int keyLen = RSA_size(m_publicKey);

    cout << "pubKey len:" << keyLen << endl;

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
        cout << "ret:" << ret << ", keyLen:" << keyLen << endl;
        retstr = toBase64(encode, ret);
    }
    else
    {
        ERR_print_errors_fp(stdout);
    }

    // 释放资源
    delete[] encode;
    return retstr;
}

string RsaCrypto::rsaPriKeyDecrypt(string data)
{
    // text指向的内存需要释放
    char* text = fromBase64(data);
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
    else
    {
        cout << "私钥解密失败..." << endl;
        ERR_print_errors_fp(stdout);
    }

    // 释放资源
    delete[] decode;
    delete[] text;
    return retstr;
}

// 签名
string RsaCrypto::rsaSign(string data, SignLevel level)
{
    unsigned int len;
    cout << "私钥长度: " << RSA_size(m_privateKey) << endl;
    char *signBuf = new char[1024];
    memset(signBuf, 0, 1024);
    int ret = RSA_sign(level, (const unsigned char*)data.data(), 
                       data.size(),(unsigned char*)signBuf, 
                       &len, m_privateKey);
    if(-1 == ret)
    {
        ERR_print_errors_fp(stdout);
    }
    cout<< "sign len:" << len << ", ret:" << ret << endl;
    string retstr = toBase64(signBuf, len);
    delete []signBuf;
    return retstr;
}

bool RsaCrypto::rsaVerify(string data, string signData, SignLevel level)
{
    // 验证签名
    int keyLen = RSA_size(m_publicKey);
    char* sign = fromBase64(signData);
    cout << RSA_size(m_publicKey) << endl;
    cout << "data.data():" << (const unsigned char*)data.data() << "\tsize" << data.size() << endl;
    cout << "signData" << (const unsigned char *)signData.data() << "\tsize" << signData.size() <<endl; 
    int ret = RSA_verify(level, (const unsigned char*)data.data(),
                         data.size(),(const unsigned char*)sign, keyLen, 
                         m_privateKey);
    cout << ret << endl;
    delete[] sign;
    if(-1 == ret)
    {
        ERR_print_errors_fp(stdout);
    }
    if(ret != 1)
    {
        return false;
    }
    return true;
}

string RsaCrypto::toBase64(const char* str, int len)
{
    BIO* mem = BIO_new(BIO_s_mem());
    BIO* bs64 = BIO_new(BIO_f_base64());

    // mem添加到bs64中
    bs64 = BIO_push(bs64, mem);

    // 写数据
    BIO_write(bs64, str, len);
    BIO_flush(bs64);

    // 得到内存对象指针
    BUF_MEM *memPtr;
    BIO_get_mem_ptr(bs64, &memPtr);
    string retStr = string(memPtr->data, memPtr->length - 1);
    BIO_free_all(bs64);
    return retStr;
}

char* RsaCrypto::fromBase64(string str)
{
    int length = str.size();
    BIO* bs64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new_mem_buf(str.data(), length);
    BIO_push(bs64, mem);

    char* buffer = new char[length];
    memset(buffer, 0, length);

    BIO_read(bs64, buffer, length);
    BIO_free_all(bs64);

    return buffer;
}
