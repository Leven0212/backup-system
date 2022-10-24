#include "huffencode.h"
#include "huffdecode.h"

using namespace huffman;

void ZIP(const std::string& filename) {
    std::string destFilename = filename + "_1";
    huffEncode he;
    he.encode(filename.data(), destFilename.data());
}

void UnZIP(const std::string& filename) {
    std::string srcFilename = filename + "_1";
    huffDecode hd;
    hd.decode(srcFilename.data(), filename.data());
}