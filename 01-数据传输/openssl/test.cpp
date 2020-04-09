#include <iostream>
#include <string>
#include "Hash.h"

using namespace std;

int main()
{
    string data = "Hello world!";
    HashType type;
    type = H_MD5;
    cout << "源数据：" << data << endl;
    Hash md5(type);
    md5.addData(data);
    string result = md5.result();
    cout << "md5值：" << result << endl;

    Hash sha1(HashType::H_SHA1);
    sha1.addData(data);
    result = sha1.result();
    cout << "sha1值：" << result << endl;
 
    Hash sha224(HashType::H_SHA224);
    sha224.addData(data);
    result = sha224.result();
    cout << "sha224值：" << result << endl;
 
    Hash sha256(HashType::H_SHA256);
    sha256.addData(data);
    result = sha256.result();
    cout << "sha256值：" << result << endl;

    Hash sha384(HashType::H_SHA384);
    sha384.addData(data);
    result = sha384.result();
    cout << "sha384值：" << result << endl;
 
    Hash sha512(HashType::H_SHA512);
    sha512.addData(data);
    result = sha512.result();
    cout << "sha512值：" << result << endl;
 
    return 0;
}

