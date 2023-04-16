
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

const int githubBlockSize = 1024; // 1KB every files

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
shared_ptr<InputStream> _githubReadRawBase64(const map<string, string> &fs, int block) {
    string fileName = to_string(githubBlockSize) + "-" + to_string(block) + ".bin";
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
    string fileName = to_string(githubBlockSize) + "-" + to_string(block) + ".bin";
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
        printf("old: %s\n", old.data());
        templateString = ::sprintf(buf, templateString.data(), inputString.data(), githubShaCompute(old).data());
    }
    req.body = shared_ptr<InputStream>(new StringInputStream(string(buf)));

//    auto base64I = new Base64DecoderInputStream(new StringInputStream(inputString), githubBlockSize);
//   shared_ptr<InputStream>x = new BinaryStringInputStream(base64I);
//    ::printf("write github :\n%s\n", x->readNbytes().data());


    httpsRequest(req, rsp);

    puts(rsp.body->readNbytes(2000).data());
    rsp.body->close();
}


// base64 IO
shared_ptr<InputStream> _githubRead(const map<string, string> &fs, int block) {
    auto res = _githubReadRawBase64(fs, block);
    if (res == nullptr) {
        return nullptr;
    }
    return shared_ptr<InputStream>(new Base64DecoderInputStream(res, githubBlockSize));
}

void _githubWrite(const map<string, string> &fs, int block, shared_ptr<InputStream> in, string old) {
    auto oldBase64InputStream = shared_ptr<InputStream>(
            new Base64EncoderInputStream(shared_ptr<InputStream>(new StringInputStream(old)), old.size()));
    old = oldBase64InputStream->readNbytes();
    _githubWriteRawBase64(fs, block, shared_ptr<InputStream>(new Base64EncoderInputStream(in, githubBlockSize)), old);
}


// multiBlock IO

shared_ptr<InputStream> githubApiFsRead(const map<string, string> &fs, int start, int end) {

    if (end - start > 10 << 20) { // > 10MB
        return shared_ptr<InputStream>(new StringInputStream(""));
    }

    int startBlockIndex = start / githubBlockSize;
    int endBlockIndex = end / githubBlockSize;
    auto body = _githubRead(fs, startBlockIndex);
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
    while (start <= end) {
        int startBlockIndex = start / githubBlockSize;
        auto oldBody = _githubRead(fs, startBlockIndex);

        string old = oldBody == nullptr ? "" : oldBody->readNbytes();
        string all = in->readNbytes(end - start + 1);
        if (all.size() != end - start + 1) {
            printf("ERROR write data, all.size()!=end-start+1");
            return;
        }
        string newData = old.size() == githubBlockSize ? old : string(githubBlockSize, '\0');
        for (int i = start; i <= end; i++) {
            newData[i % githubBlockSize] = all[i - start];
        }
        auto writeData = shared_ptr<InputStream>(new StringInputStream(newData));
        _githubWrite(fs, startBlockIndex, writeData, old);


        start = (startBlockIndex + 1) * githubBlockSize;

    }
}







