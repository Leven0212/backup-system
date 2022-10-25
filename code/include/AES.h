
#ifndef __AES_H__
#define __AES_H__

#define AES_BIT 256 // 192//256
#define AES_KEYLEN AES_BIT / 8

enum SysErrCode {
    ErrNone = 0,
    ErrSrcNotExist,
    ErrSrcOpenFail,
    ErrDstOpenFail,
    ErrWriteFail,
    ErrAesFileFormat
};

class aes {
  public:
    enum AESBIT {
        AES_256 = 256,
        AES_192 = 192,
        AES_128 = 128,
    };

    int aesBit;
    int aesKeyLen;
    unsigned char *myKey;

    aes();
    ~aes();

    int setKey(unsigned char *key, AESBIT bit);

    int encryptFile(const char *src_path, const char *dst_path);
    int decryptFile(const char *src_path, const char *dst_path);
};

#endif