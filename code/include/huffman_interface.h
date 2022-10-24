#include "huffman/huffencode.h"
#include "huffman/huffdecode.h"

using namespace huffman;

void ZIP(const std::string &filename) {
    std::string destFilename = filename + ".huf";
    std::string src = filename;
    huffEncode he;
    he.encode(src.data(), destFilename.data());
}

void UnZIP(const std::string &filename) {
    std::string srcFilename = filename + ".huf";
    std::string dst = filename;
    huffDecode hd;
    hd.decode(srcFilename.data(), dst.data());
}