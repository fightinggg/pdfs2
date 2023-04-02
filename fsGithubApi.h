
#include "allheader.h"
#include "stream.h"
#include "httpClient.h"

void githubApiFswrite(const map<string, string> &fs, const string &name, InputStream *inputStream) {
    HttpReq req;
    HttpRsp rsp;
    req.method = "GET";
    req.url = "/";
    req.host = "140.82.113.4";
    req.port = 80;

    httpRequest(req, rsp);
    puts(rsp.body->readNbytes(2000).data());
    rsp.body->close();
    delete rsp.body;
}


InputStream *githubApiFsRead(const map<string, string> &fs, const string &name) {
    string githubUsername = "fightinggg";
    string githubRepoName = "pdfs-data";
    string fileName = "a.txt";

    //TODO https

    HttpReq req;
    HttpRsp rsp;
    req.method = "GET";
    req.url = "/repos/" + githubUsername + "/" + githubRepoName + "/contents/" + fileName;
    req.host = "api.github.com";
    req.port = 80;

    httpRequest(req, rsp);
    return rsp.body;
//    puts(rsp.body->readNbytes(2000).data());
//    rsp.body->close();
//    delete rsp.body;
}