//#include <fstream>
#include <AES.h>
typedef bitset<8> byte;
typedef bitset<32> word;
/**
 *  将一个char字符数组转化为二进制
 *  存到一个 byte 数组中
 */
void charToByte(byte out[16], const char s[16])
{ 
	for(int i=0; i<16; ++i)  
		for(int j=0; j<8; ++j)  
			out[i][j]= ((s[i]>>j) & 1);  
}

/**
 *  将连续的128位分成16组，存到一个 byte 数组中
 */
void divideToByte(byte out[16], bitset<128>& data)
{
	bitset<128> temp;
	for(int i=0; i<16; ++i)
	{
		temp = (data << 8*i) >> 120;
		out[i] = temp.to_ulong();
	}
}

/**
 *  将16个 byte 合并成连续的128位
 */
bitset<128> mergeByte(byte in[16])
{
	bitset<128> res;
	res.reset();  // 置0
	bitset<128> temp;
	for(int i=0; i<16; ++i)
	{
		temp = in[i].to_ulong();
		temp <<= 8*(15-i);
		res |= temp;
	}
	return res;
}

bool Encrypt(string src, string key)
{
	string tar = src;
	byte key[16]; 
	charToByte(key, keyStr.c_str());
	// 密钥扩展
	word w[4*(Nr+1)];
	KeyExpansion(key, w);

	bitset<128> data;
	byte plain[16];
	// 将文件 flower.jpg 加密到 cipher.txt 中
	ifstream in;
	ofstream out;

	in.open(src, ios::binary);
	out.open(tar, ios::binary);
	while(in.read((char*)&data, sizeof(data)))
	{
		divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		out.write((char*)&data, sizeof(data));
		data.reset();  // 置0
	}
	in.close();
	out.close();
	return true;
}

bool Decrypt(string src, string key)
{
	string tar = src;
	byte key[16]; 
	charToByte(key, keyStr.c_str());
	// 密钥扩展
	word w[4*(Nr+1)];
	KeyExpansion(key, w);

	bitset<128> data;
	byte plain[16];
	// 将文件 flower.jpg 加密到 cipher.txt 中
	ifstream in;
	ofstream out;
	
	in.open("D://cipher.txt", ios::binary);
	out.open("D://flower1.jpg", ios::binary);
	while(in.read((char*)&data, sizeof(data)))
	{
		divideToByte(plain, data);
		decrypt(plain, w);
		data = mergeByte(plain);
		out.write((char*)&data, sizeof(data));
		data.reset();  // 置0
	}
	in.close();
	out.close();
}