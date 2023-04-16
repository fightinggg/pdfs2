
#include <utility>

#include "allheader.h"
#include "stream.h"
#include "httpClient.h"
#include "Supplayer.h"
#include "inputstreams/Base64InputStream.h"
#include "libsrc/sha1.h"
#include "httpcodec/decode.h"
#include "inputstreams/BinaryStringInputStream.h"
#include "inputstreams/SplitNewLineStream.h"

const int githubBlockSize = 10240; // 10KB every files
const bool debug = false;


int readCnt = 0;
int writeCnt = 0;

string encodeFileName(int block) {
    // github can save as most 500MB, so here is 500*1024 = 512000 files
    static char buf[30];
    sprintf(buf, "%d-%010d.bin", githubBlockSize, block);
    return string(buf);
}

string toHex(const string &old) {
    string res;
    static string hexMap = "0123456789abcdef";
    for (auto x: old) {
        res += hexMap[x & 0xff];
    }
    return res;
}

// sha1
string toSHAHex(const string &old) {
    SHA1 checksum;
    checksum.update(old);
    return checksum.final();
}


string githubShaCompute(const string &old) {
    return toSHAHex("blob " + to_string(old.size()) + '\0' + old);
}


// raw IO
string datamem(100 << 20, 'A');

shared_ptr<InputStream> _githubReadRawBase64(const map<string, string> &fs, int block) {
    readCnt++;
    printf("readCnt=%d\n", readCnt);
    if (debug) {
        int githubBase64Block = (githubBlockSize + 2) / 3 * 4;
        auto res = shared_ptr<InputStream>(
                new StringInputStream(
                        datamem.substr(block * githubBase64Block, block * githubBase64Block + githubBase64Block)));
        return res;
    }

    string fileName = encodeFileName(block);
    string githubUsername = "fightinggg";
    string githubRepoName = "pdfs-data-githubapi";
    string githubToken = fs.at("githubToken");

    HttpReq req;
    HttpRsp rsp;
    req.method = "GET";
    req.url = "/repos/" + githubUsername + "/" + githubRepoName + "/contents/" + fileName;
    req.host = "api.github.com";
    req.port = 443;
    req.body = nullptr;

    req.headers["Authorization"] = "Bearer " + githubToken;
    req.headers["Accept"] = "application/vnd.github.v3.raw";
    req.headers["X-GitHub-Api-Version"] = "2022-11-28";
    req.headers["User-Agent"] = "libcurl";
    httpsRequest(req, rsp);

    string s = rsp.body->readNbytes();
    rsp.body = shared_ptr<InputStream>(new StringInputStream(s));
//    exit(-1);

    auto body = rsp.body;
    rsp.body = nullptr;
    decodeHttpRsp(body, rsp.status, rsp.headers, rsp.body);

    string s2 = rsp.body->readNbytes();
    rsp.body = shared_ptr<InputStream>(new StringInputStream(s2));

    if (s2.length() < 10000 && rsp.status != 404) {
        int a = 0;
        a++;
    }

    if (rsp.status == 404) {
        return nullptr;
    } else if (rsp.status == 200) {
        return rsp.body;
    } else {
        printf("ERROR: github return status=%d", rsp.status);
        string s = rsp.body->readNbytes();
        rsp.body = shared_ptr<InputStream>(new StringInputStream(s));
        printf("body=%s", s.data());
        exit(-1);
    }

}

void _githubWriteRawBase64(const map<string, string> &fs, int block, shared_ptr<InputStream> in, string old) {
    writeCnt++;
    printf("writeCnt=%d\n", writeCnt);

    if (debug) {
        int githubBase64Block = (githubBlockSize + 2) / 3 * 4;
        auto s = in->readNbytes();
        int x = s.size();
        if (x != githubBase64Block) {
            exit(-1);
        }
        for (int i = 0; i < githubBase64Block; i++) {
            datamem[block * githubBase64Block + i] = s.at(i);
        }
        return;
    }


    string fileName = encodeFileName(block);
    string githubUsername = "fightinggg";
    string githubRepoName = "pdfs-data-githubapi";
    string githubToken = fs.at("githubToken");


    HttpReq req;
    HttpRsp rsp;
    req.method = "PUT";
    req.url = "/repos/" + githubUsername + "/" + githubRepoName + "/contents/" + fileName;
    req.host = "api.github.com";
    req.port = 443;

    req.headers["Authorization"] = "Bearer " + githubToken;
    req.headers["Accept"] = "application/vnd.github+json";
    req.headers["X-GitHub-Api-Version"] = "2022-11-28";
    req.headers["Content-Type"] = "application/json";
    req.headers["User-Agent"] = "libcurl";

    string templateString;
    char buf[githubBlockSize / 3 * 4 / 3 * 4 + 300]; // base64 size + 300

    if (in->size() == -1) {
        exit(-1);
    }
    auto tmpI = shared_ptr<InputStream>(new Base64EncoderInputStream(in, in->size()));
    string inputString = tmpI->readNbytes();

    if (old.empty()) {
        templateString = R""(
            {
                "message": "pdfs-github-api-upload",
                "content": "%s"
            }
            )"";
        templateString = ::sprintf(buf, templateString.data(), inputString.data());
    } else {
        templateString = R""(
            {
                "message": "pdfs-github-api-upload",
                "content": "%s",
                "sha": "%s"
            }
            )"";
//        printf("old: %s\n", old.data());
        templateString = ::sprintf(buf, templateString.data(), inputString.data(), githubShaCompute(old).data());
    }
    req.body = shared_ptr<InputStream>(new StringInputStream(string(buf)));

//    auto base64I = new Base64DecoderInputStream(new StringInputStream(inputString), githubBlockSize);
//   shared_ptr<InputStream.h>x = new BinaryStringInputStream(base64I);
//    ::printf("write github :\n%s\n", x->readNbytes().data());


    httpsRequest(req, rsp);


    auto body = rsp.body;
    rsp.body = nullptr;
    decodeHttpRsp(body, rsp.status, rsp.headers, rsp.body);

    if (rsp.status == 404) {
    } else if (rsp.status == 201) {
    } else if (rsp.status == 200) {
    } else {
        printf("ERROR: github return status=%d", rsp.status);
        string s = rsp.body->readNbytes();
        rsp.body = shared_ptr<InputStream>(new StringInputStream(s));
        printf("body=%s", s.data());
        exit(-1);
    }


    rsp.body->close();
}


// base64 IO

//char buf[100 << 20];

// 读取二进制数据
shared_ptr<InputStream> _githubRead(const map<string, string> &fs, int block) {
//    return shared_ptr<InputStream>(new StringInputStream(
//            string(buf + block * githubBlockSize, buf + block * githubBlockSize + githubBlockSize)));
//

    auto res = _githubReadRawBase64(fs, block);
    if (res == nullptr) {
        return nullptr;
    }
    return shared_ptr<InputStream>(new Base64DecoderInputStream(res, githubBlockSize));
}

// 写入二进制数据
void _githubWrite(const map<string, string> &fs, int block, shared_ptr<InputStream> in, string old) {
//    in = shared_ptr<InputStream>(
//            new Base64DecoderInputStream(shared_ptr<InputStream>(new Base64EncoderInputStream(in, githubBlockSize)),
//                                         githubBlockSize));
//    for (int i = block * githubBlockSize; i < block * githubBlockSize + githubBlockSize; i++) {
//        in->read(buf + i);
//    }
//    return;


    auto oldBase64InputStream = shared_ptr<InputStream>(
            new Base64EncoderInputStream(shared_ptr<InputStream>(new StringInputStream(old)), old.size()));
    old = oldBase64InputStream->readNbytes();
    _githubWriteRawBase64(fs, block, shared_ptr<InputStream>(new Base64EncoderInputStream(in, githubBlockSize)), old);
}


// empty IO// empty IO// empty IO// empty IO// empty IO// empty IO// empty IO
bitset<500 * 1024> unInit;
bool emptySetInit = false;

shared_ptr<InputStream> _githubReadEmpty(const map<string, string> &fs, int block) {


    if (!emptySetInit) {
        // 临时使用，每次程序启动后，清楚所有文件
        emptySetInit = true;
        for (int i = 0; i < unInit.size(); i++) {
            unInit[i] = true;
        }
    }
    if (unInit[block]) {
        return shared_ptr<InputStream>(new StringInputStream(string(githubBlockSize, '\0')));
    } else {
        return _githubRead(fs, block);
    }
}

void _githubWriteEmpty(const map<string, string> &fs, int block, shared_ptr<InputStream> in, string old) {

    bool isAllZero = true;
    auto all = in->readNbytes();
    for (int i = 0; i < all.size(); i++) {
        if (all[i] != 0) {
            isAllZero = false;
            break;
        }
    }
    if (!isAllZero || !unInit[block]) {
        unInit[block] = false;
        auto oldI = _githubRead(fs, block);
        old = oldI == nullptr ? "" : oldI->readNbytes();
        return _githubWrite(fs, block, shared_ptr<InputStream>(new StringInputStream(all)), old);
    }
}




// multiBlock IO

shared_ptr<InputStream> githubApiFsRead(const map<string, string> &fs, int start, int end) {

    if (end - start > 10 << 20) { // > 10MB
        return shared_ptr<InputStream>(new StringInputStream(""));
    }

    int startBlockIndex = start / githubBlockSize;
    int endBlockIndex = end / githubBlockSize;
    auto body = _githubReadEmpty(fs, startBlockIndex);
    if (body == nullptr) {
        body = shared_ptr<InputStream>(new StringInputStream(string(githubBlockSize, '\0')));
    }

    if (startBlockIndex == endBlockIndex) {
        return shared_ptr<InputStream>(new SubInputStream(body, start % githubBlockSize, end - start + 1));
    } else {
        int size = githubBlockSize - (start % githubBlockSize);
        shared_ptr<InputStream> now = shared_ptr<InputStream>(new SubInputStream(body, start % githubBlockSize, size));
        class GithubGetter : public Supplayer<shared_ptr<InputStream> > {
            map<string, string> fs{};
            int start;
            int end;

        public:
            shared_ptr<InputStream> get() override {
                return githubApiFsRead(fs, start, end);
            }

            GithubGetter(map<string, string> fs, int start, int end) {
                this->fs = std::move(fs);
                this->start = start;
                this->end = end;
            }
        };
        int newStart = (start + githubBlockSize) / githubBlockSize * githubBlockSize;
        shared_ptr<Supplayer<shared_ptr<InputStream>>> next(new GithubGetter(fs, newStart, end));
        return shared_ptr<InputStream>(new MergeInputStream(now, next, end - start + 1));
    }
}

void githubApiFswrite(const map<string, string> &fs, int start, int end, const shared_ptr<InputStream> &in) {
    string all = in->readNbytes(end - start + 1);
    if (all.size() != end - start + 1) {
        printf("ERROR write data, all.size()!=end-tmpStart+1, %ld!=%d", all.size(), end - start + 1);
        return;
    }

    int tmpStart = start;
    while (tmpStart <= end) {
        int startBlockIndex = tmpStart / githubBlockSize;
        int tmpend = min(startBlockIndex * githubBlockSize + githubBlockSize - 1, end);
        auto oldBody = _githubReadEmpty(fs, startBlockIndex);

        string old = oldBody == nullptr ? "" : oldBody->readNbytes();

        string newData = old.size() == githubBlockSize ? old : string(githubBlockSize, '\0');
        for (int i = tmpStart; i <= tmpend; i++) {
            newData[i % githubBlockSize] = all.at(i - start);
        }
        auto writeData = shared_ptr<InputStream>(new StringInputStream(newData));
        _githubWriteEmpty(fs, startBlockIndex, writeData, old);


        tmpStart = (startBlockIndex + 1) * githubBlockSize;

    }
}







