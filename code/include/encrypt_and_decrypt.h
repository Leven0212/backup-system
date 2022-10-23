#include <fstream>
#include "AES.h"
#include <unistd.h>

/**
 *  将一个char字符数组转化为二进制
 *  存到一个 byte 数组中
 */
void charToByte(byte out[16], const char s[16]) {
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 8; ++j)
            out[i][j] = ((s[i] >> j) & 1);
}

/**
 *  将连续的128位分成16组，存到一个 byte 数组中
 */
void divideToByte(byte out[16], std::bitset<128> &data) {
    std::bitset<128> temp;
    for (int i = 0; i < 16; ++i) {
        temp = (data << 8 * i) >> 120;
        out[i] = temp.to_ulong();
    }
}

/**
 *  将16个 byte 合并成连续的128位
 */
std::bitset<128> mergeByte(byte in[16]) {
    std::bitset<128> res;
    res.reset();
    std::bitset<128> temp;
    for (int i = 0; i < 16; ++i) {
        temp = in[i].to_ulong();
        temp <<= 8 * (15 - i);
        res |= temp;
    }
    return res;
}

bool Encrypt(std::string src, std::string keyStr) {
    std::string tar = src + ".cpt";
    byte key[16];
    charToByte(key, keyStr.c_str());

    word w[4 * (Nr + 1)];
    KeyExpansion(key, w);

    std::bitset<128> data;
    byte plain[16];

    std::ifstream in;
    std::ofstream out;

    in.open(src, std::ios::binary);
    out.open(tar, std::ios::binary);
    while (in.read((char *)&data, sizeof(data))) {
        divideToByte(plain, data);
        encrypt(plain, w);
        data = mergeByte(plain);
        out.write((char *)&data, sizeof(data));
        data.reset();
    }
    in.close();
    out.close();
    return true;
}

bool Decrypt(std::string src, std::string keyStr) {
    std::string tar = src;
    src = src + ".cpt";
    byte key[16];
    charToByte(key, keyStr.c_str());

    word w[4 * (Nr + 1)];
    KeyExpansion(key, w);

    std::bitset<128> data;
    byte plain[16];

    std::ifstream in;
    std::ofstream out;

    in.open(src, std::ios::binary);
    out.open(tar, std::ios::binary);
    while (in.read((char *)&data, sizeof(data))) {
        divideToByte(plain, data);
        decrypt(plain, w);
        data = mergeByte(plain);
        out.write((char *)&data, sizeof(data));
        data.reset(); // 置0
    }
    in.close();
    out.close();
    return true;
}