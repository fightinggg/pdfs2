
#include "allheader.h"
#include "stream.h"
#include "httpClient.h"

void githubApiFswrite(const map<string, string> &fs, const string &name, InputStream *inputStream) {
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


InputStream *githubApiFsRead(const map<string, string> &fs, const string &name) {
    string githubUsername = "fightinggg";
    string githubRepoName = "pdfs-data-githubapi";
    string fileName = "-kM8Jt+BpnR3LBV-i4Fh5Q==.bin";
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
    return rsp.body;
}