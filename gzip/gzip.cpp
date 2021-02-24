/*
 * @Description:
 * @Author: Hejun Jiang
 * @Date: 2021-02-23 09:36:44
 * @LastEditTime: 2021-02-23 17:12:20
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */
#include <zlib.h>

#include <fstream>
#include <sstream>

#define MULIT 4

std::string gzCompress(std::string src) {
    z_stream c_stream;
    if (!src.empty()) {
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return "";
        c_stream.next_in = (Bytef *)(src.c_str());
        c_stream.avail_in = src.size();
        int destLen = src.size() * MULIT;
        char *dest = new char[destLen];
        c_stream.next_out = (Bytef *)dest;
        c_stream.avail_out = destLen;
        while (c_stream.avail_in != 0 && c_stream.total_out < destLen) {
            if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return "";
        }
        if (c_stream.avail_in != 0) return std::string(dest, c_stream.avail_in);
        for (;;) {
            int err = 0;
            if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
            if (err != Z_OK) return "";
        }
        if (deflateEnd(&c_stream) != Z_OK) return "";
        return std::string(dest, c_stream.total_out);
    }
    return "";
}

std::string gzDecompress(std::string src) {
    z_stream strm;
    strm.zalloc = NULL;
    strm.zfree = NULL;
    strm.opaque = NULL;
    strm.next_in = (Bytef *)(src.c_str());
    strm.avail_in = src.size();
    int destLen = src.size() * MULIT;
    char *dest = new char[destLen];
    strm.next_out = (Bytef *)dest;
    strm.avail_out = destLen;
    if (inflateInit2(&strm, MAX_WBITS + 16) == Z_OK && inflate(&strm, Z_FINISH) == Z_STREAM_END) return std::string(dest, strm.total_out);
    return "";
}

int main() {
    // std::string str = "hello world!!!!";
    std::ifstream fin("./5057.wav", std::ios::in);
    std::stringstream buf;
    buf << fin.rdbuf();
    fin.close();
    std::string str = buf.str();
    std::string dest = gzCompress(str);
    if (!dest.empty()) {
        // printf("Compress:%s\n", str.c_str());
        printf("Compress size: %lu\n", dest.size());
        std::ofstream fout("./ctest.gz", std::ios::out);
        fout << dest;
        fout.close();

        std::string src = gzDecompress(dest);
        if (!src.empty()) {
            // printf("uncompressed:%s\n", src.c_str());
            printf("uncompressed size: %lu\n", src.size());
        }
    }
    return 0;
}