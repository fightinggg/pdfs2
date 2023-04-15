
#include "allheader.h"
#include "stream.h"
#include "httpClient.h"
#include "Supplayer.h"

int githubBlockSize = 1024; // 1KB every files

void githubApiFswrite(const map<string, string> &fs, int start, int end, InputStream *inputStream) {
    HttpReq req;
    HttpRsp rsp;
    req.method = "GET";
    req.url = "/";
    req.host = "140.82.113.4";
    req.port = 443;

    httpsRequest(req, rsp);
    puts(rsp.body->readNbytes(2000).data());
    rsp.body->close();
    delete rsp.body;
}


InputStream *githubApiFsRead(const map<string, string> &fs, int start, int end) {
    int startBlockIndex = start / githubBlockSize;
    int endBlockIndex = end / githubBlockSize;
    string fileName = to_string(githubBlockSize) + "-" + to_string(startBlockIndex) + ".bin";
    fileName = "-kM8Jt+BpnR3LBV-i4Fh5Q==.bin";

    string githubUsername = "fightinggg";
    string githubRepoName = "pdfs-data-githubapi";
    string githubToken = fs.at("githubToken");

    HttpReq req;
    HttpRsp rsp;
    req.method = "GET";
    req.url = "/repos/" + githubUsername + "/" + githubRepoName + "/contents/" + fileName;
    req.host = "api.github.com";
    req.port = 443;

    req.headers["Authorization"] = "Bearer " + githubToken;
    req.headers["Accept"] = "application/vnd.github.v3.raw";
    req.headers["X-GitHub-Api-Version"] = "2022-11-28";
    req.headers["User-Agent"] = "libcurl";
    httpsRequest(req, rsp);
//    string s;
//    srand(time(nullptr));
//    for (int i = 0; i < 3000; i++) {
//        if (rand() % 2 == 0) {
//            s += '#';
//        } else {
//            s += ' ';
//        }
//    }
//    rsp.body = new StringInputStream(s);

    if (startBlockIndex == endBlockIndex) {
        return new SubInputStream(rsp.body, start % githubBlockSize, end - start + 1);
    } else {
        int size = githubBlockSize - (start % githubBlockSize);
//        string all = rsp.body->readNbytes();
//        delete rsp.body;
//        rsp.body = new StringInputStream(all);
//        printf("rsp.body.size=%ld\n",all.size());
        InputStream *now = new SubInputStream(rsp.body, start % githubBlockSize, size);
        class GithubGetter : public Supplayer<InputStream> {
            map<string, string> fs{};
            int start;
            int end;

        public:
            InputStream *get() override {
                return githubApiFsRead(fs, start, end);
            }

            GithubGetter(map<string, string> fs, int start, int end) {
                this->fs = fs;
                this->start = start;
                this->end = end;
            }
        };
        int newStart = (start + githubBlockSize) / githubBlockSize * githubBlockSize;
        Supplayer<InputStream> *next = new GithubGetter(fs, newStart, end);
        return new MergeInputStream(now, next, end - start + 1);
    }
}



